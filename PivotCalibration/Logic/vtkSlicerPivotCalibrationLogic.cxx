/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// PivotCalibration Logic includes
#include "vtkSlicerPivotCalibrationLogic.h"

// MRML includes
#include <vtkMRMLLinearTransformNode.h>
#include "vtkMRMLScene.h"

// vtkIGSIOCalibration includes
#include <vtkIGSIOPivotCalibrationAlgo.h>
#include <vtkIGSIOSpinCalibrationAlgo.h>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkNew.h>
#include <vtkEventBroker.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkTransform.h>

// STD includes
#include <cassert>
#include <cmath>

// VNL includes
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_determinant.h"

static const double PARALLEL_ANGLE_THRESHOLD_DEGREES = 20.0;
// Note: If the needle orientation protocol changes, only the definitions of shaftAxis and secondaryAxes need to be changed
// Define the shaft axis and the secondary shaft axis
// Current needle orientation protocol dictates: shaft axis -z, orthogonal axis +x
// If StylusX is parallel to ShaftAxis then: shaft axis -z, orthogonal axis +y
static const double SHAFT_AXIS[3] = { 0, 0, -1 };
static const double ORTHOGONAL_AXIS[3] = { 1, 0, 0 };
static const double BACKUP_AXIS[3] = { 0, 1, 0 };

class vtkSlicerPivotCalibrationLogic::vtkInternal
{
public:
  vtkInternal(vtkSlicerPivotCalibrationLogic* external)
  {
    this->External = external;
  }
  ~vtkInternal() = default;

  vtkSlicerPivotCalibrationLogic* External;
  vtkNew<vtkIGSIOPivotCalibrationAlgo> PivotCalibrationAlgo;
  vtkNew<vtkIGSIOSpinCalibrationAlgo> SpinCalibrationAlgo;
  vtkNew<vtkCallbackCommand>            PivotCalibrationCallbackCommand;

  vtkIGSIOAbstractCalibrationAlgo* GetCurrentCalibrationAlgo();
};

//----------------------------------------------------------------------------
vtkIGSIOAbstractCalibrationAlgo* vtkSlicerPivotCalibrationLogic::vtkInternal::GetCurrentCalibrationAlgo()
{
  switch (this->External->GetCalibrationMode())
  {
  case PIVOT_CALIBRATION:
    return this->PivotCalibrationAlgo;
    break;
  case SPIN_CALIBRATION:
    return this->SpinCalibrationAlgo;
    break;
  default:
    vtkWarningWithObjectMacro(this->External, "Could not get the calibration algorithm for the specified calibration mode");
    break;
  }
  return nullptr;
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerPivotCalibrationLogic);

//----------------------------------------------------------------------------
vtkSlicerPivotCalibrationLogic::vtkSlicerPivotCalibrationLogic()
{
  this->Internal = new vtkInternal(this);
  this->ToolTipToToolMatrix = vtkMatrix4x4::New();
  this->ObservedTransformNode = NULL;
  this->RecordingState = false;

  this->Internal->PivotCalibrationCallbackCommand->SetCallback(vtkSlicerPivotCalibrationLogic::ProcessPivotCalibrationAlgorithmEvents);
  this->Internal->PivotCalibrationCallbackCommand->SetClientData(reinterpret_cast<void*>(this));
  vtkEventBroker::GetInstance()->AddObservation(this->Internal->PivotCalibrationAlgo, vtkCommand::ModifiedEvent, this, this->Internal->PivotCalibrationCallbackCommand);
  vtkEventBroker::GetInstance()->AddObservation(this->Internal->PivotCalibrationAlgo, vtkIGSIOPivotCalibrationAlgo::InputTransformAddedEvent, this, this->Internal->PivotCalibrationCallbackCommand);
  vtkEventBroker::GetInstance()->AddObservation(this->Internal->SpinCalibrationAlgo, vtkCommand::ModifiedEvent, this, this->Internal->PivotCalibrationCallbackCommand);
  vtkEventBroker::GetInstance()->AddObservation(this->Internal->SpinCalibrationAlgo, vtkIGSIOPivotCalibrationAlgo::InputTransformAddedEvent, this, this->Internal->PivotCalibrationCallbackCommand);
}

//----------------------------------------------------------------------------
vtkSlicerPivotCalibrationLogic::~vtkSlicerPivotCalibrationLogic()
{
  delete this->Internal;
  this->ToolTipToToolMatrix->Delete();
  this->SetAndObserveTransformNode(NULL); // Remove the observer
}

//----------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::ProcessPivotCalibrationAlgorithmEvents(vtkObject* vtkNotUsed(caller), unsigned long event, void* clientData, void* vtkNotUsed(callData))
{
  vtkSlicerPivotCalibrationLogic* self = reinterpret_cast<vtkSlicerPivotCalibrationLogic*>(clientData);

  if (event == vtkCommand::ModifiedEvent)
  {
    self->InvokeEvent(vtkCommand::ModifiedEvent);
  }
  else if (event == vtkIGSIOPivotCalibrationAlgo::InputTransformAddedEvent)
  {
    self->InvokeEvent(InputTransformAdded);
  }
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* vtkNotUsed(callData))
{
  if (caller != NULL)
  {
    vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(caller);
    if (event == vtkMRMLLinearTransformNode::TransformModifiedEvent && this->RecordingState == true && strcmp(transformNode->GetID(), this->ObservedTransformNode->GetID()) == 0)
    {
      vtkNew<vtkMatrix4x4> toolToReferenceMatrix;
      transformNode->GetMatrixTransformToParent(toolToReferenceMatrix);
      this->AddToolToReferenceMatrix(toolToReferenceMatrix);
    }
  }
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetAndObserveTransformNode(vtkMRMLLinearTransformNode* transformNode)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLLinearTransformNode::TransformModifiedEvent);
  vtkSetAndObserveMRMLNodeEventsMacro(this->ObservedTransformNode, transformNode, events.GetPointer());
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::AddToolToReferenceMatrix(vtkMatrix4x4* transformMatrix)
{
  if (!transformMatrix)
  {
    vtkErrorMacro("vtkSlicerPivotCalibrationLogic::AddToolToReferenceMatrix failed: invalid transformMatrix");
    return;
  }

  if (this->GetCalibrationMode() == PIVOT_CALIBRATION)
  {
    this->Internal->PivotCalibrationAlgo->InsertNextCalibrationPoint(transformMatrix);
  }
  else
  {
    this->Internal->SpinCalibrationAlgo->InsertNextCalibrationPoint(transformMatrix);
  }

  if (this->AutoCalibrationEnabled && this->GetNumberOfPoses() >= this->AutoCalibrationNumberOfPoints)
  {
    this->ComputeAutoCalibration();
  }
}

//---------------------------------------------------------------------------
bool vtkSlicerPivotCalibrationLogic::ComputeAutoCalibration()
{
  bool success = false;
  double error = -1.0;
  if (this->CalibrationMode == PIVOT_CALIBRATION)
  {
    success = this->ComputePivotCalibration();
    error = this->PivotRMSE;
  }
  else if (this->CalibrationMode == SPIN_CALIBRATION)
  {
    success = this->ComputeSpinCalibration();
  }

  if (!success || error >= this->AutoCalibrationStopTargetError)
  {
    return false;
  }

  if (this->AutoCalibrationStopWhenComplete)
  {
    this->SetRecordingState(false);
  }

  this->InvokeEvent(vtkSlicerPivotCalibrationLogic::CalibrationCompleteEvent);
  return true;
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::ClearToolToReferenceMatrices()
{
  this->Internal->PivotCalibrationAlgo->RemoveAllCalibrationPoints();
  this->Internal->SpinCalibrationAlgo->RemoveAllCalibrationPoints();
}

//---------------------------------------------------------------------------
int vtkSlicerPivotCalibrationLogic::GetErrorCode()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return -1;
  }
  return algo->GetErrorCode();
}

//---------------------------------------------------------------------------
std::string vtkSlicerPivotCalibrationLogic::GetErrorText()
{
  std::string errorText;
  switch (this->GetErrorCode())
  {
  case vtkIGSIOAbstractCalibrationAlgo::CALIBRATION_SUCCESS:
  case vtkIGSIOAbstractCalibrationAlgo::CALIBRATION_NOT_STARTED:
    break;
  case vtkIGSIOAbstractCalibrationAlgo::CALIBRATION_NOT_ENOUGH_VARIATION:
    errorText = "Couldn't perform calibration; not enough variation";
    break;
  case vtkIGSIOAbstractCalibrationAlgo::CALIBRATION_NOT_ENOUGH_POINTS:
    errorText = "Couldn't perform calibration; not enough points";
    break;
  case vtkIGSIOAbstractCalibrationAlgo::CALIBRATION_HIGH_ERROR:
    errorText = "Couldn't perform calibration; error is too high";
    break;
  default:
  case vtkIGSIOAbstractCalibrationAlgo::CALIBRATION_FAIL:
    errorText = "Couldn't perform calibration";
    break;
  }
  return errorText;
}

//---------------------------------------------------------------------------
bool vtkSlicerPivotCalibrationLogic::ComputePivotCalibration(bool autoOrient /*=true*/)
{
  vtkNew<vtkMatrix4x4> toolTipToToolMatrix;
  toolTipToToolMatrix->DeepCopy(this->ToolTipToToolMatrix);
  this->Internal->PivotCalibrationAlgo->SetPivotPointToMarkerTransformMatrix(toolTipToToolMatrix);

  if (this->Internal->PivotCalibrationAlgo->DoPivotCalibration(nullptr, autoOrient) != IGSIO_SUCCESS)
  {
    this->SetPivotRMSE(-1.0);
    vtkErrorMacro("ComputePivotCalibration: " << this->GetErrorText());
    return false;
  }

  //set the RMSE
  this->SetPivotRMSE(this->Internal->PivotCalibrationAlgo->GetPivotCalibrationErrorMm());

  //set the transformation
  this->ToolTipToToolMatrix->DeepCopy(
    this->Internal->PivotCalibrationAlgo->GetPivotPointToMarkerTransformMatrix());

  return true;
}

//---------------------------------------------------------------------------
bool vtkSlicerPivotCalibrationLogic::ComputeSpinCalibration(bool snapRotation /*=false*/, bool autoOrient /*=true*/)
{
  vtkNew<vtkMatrix4x4> toolTipToToolMatrix;
  toolTipToToolMatrix->DeepCopy(this->ToolTipToToolMatrix);
  this->Internal->SpinCalibrationAlgo->SetPivotPointToMarkerTransformMatrix(toolTipToToolMatrix);

  if (this->Internal->SpinCalibrationAlgo->DoSpinCalibration(nullptr, snapRotation, autoOrient) != IGSIO_SUCCESS)
  {
    this->SetSpinRMSE(-1.0);
    vtkErrorMacro("ComputeSpinCalibration: " << this->GetErrorText());
    return false;
  }

  //set the RMSE
  this->SetSpinRMSE(this->Internal->SpinCalibrationAlgo->GetSpinCalibrationErrorMm());

  //set the transformation
  this->ToolTipToToolMatrix->DeepCopy(
    this->Internal->SpinCalibrationAlgo->GetPivotPointToMarkerTransformMatrix());

  return true;
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::GetToolTipToToolTranslation(vtkMatrix4x4* translationMatrix)
{
  translationMatrix->Identity();
  translationMatrix->SetElement(0, 3, this->ToolTipToToolMatrix->GetElement(0, 3));
  translationMatrix->SetElement(1, 3, this->ToolTipToToolMatrix->GetElement(1, 3));
  translationMatrix->SetElement(2, 3, this->ToolTipToToolMatrix->GetElement(2, 3));
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::GetToolTipToToolRotation(vtkMatrix4x4* rotationMatrix)
{
  rotationMatrix->Identity();
  rotationMatrix->SetElement(0, 0, this->ToolTipToToolMatrix->GetElement(0, 0));
  rotationMatrix->SetElement(0, 1, this->ToolTipToToolMatrix->GetElement(0, 1));
  rotationMatrix->SetElement(0, 2, this->ToolTipToToolMatrix->GetElement(0, 2));
  rotationMatrix->SetElement(1, 0, this->ToolTipToToolMatrix->GetElement(1, 0));
  rotationMatrix->SetElement(1, 1, this->ToolTipToToolMatrix->GetElement(1, 1));
  rotationMatrix->SetElement(1, 2, this->ToolTipToToolMatrix->GetElement(1, 2));
  rotationMatrix->SetElement(2, 0, this->ToolTipToToolMatrix->GetElement(2, 0));
  rotationMatrix->SetElement(2, 1, this->ToolTipToToolMatrix->GetElement(2, 1));
  rotationMatrix->SetElement(2, 2, this->ToolTipToToolMatrix->GetElement(2, 2));
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::GetToolTipToToolMatrix(vtkMatrix4x4* matrix)
{
  matrix->DeepCopy(this->ToolTipToToolMatrix);
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetToolTipToToolMatrix(vtkMatrix4x4* matrix)
{
  this->ToolTipToToolMatrix->DeepCopy(matrix);
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::FlipShaftDirection()
{
  vtkIGSIOAbstractCalibrationAlgo::FlipShaftDirection(this->ToolTipToToolMatrix);
}

//---------------------------------------------------------------------------
int vtkSlicerPivotCalibrationLogic::GetNumberOfPoses()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return 0;
  }
  return algo->GetNumberOfCalibrationPoints();
}

//---------------------------------------------------------------------------
double vtkSlicerPivotCalibrationLogic::GetMinimumOrientationDifferenceDeg()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return -1.0;
  }
  return algo->GetMinimumOrientationDifferenceDeg();
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetMinimumOrientationDifferenceDeg(double minimumOrientationDifferenceDeg)
{
  this->Internal->PivotCalibrationAlgo->SetMinimumOrientationDifferenceDeg(minimumOrientationDifferenceDeg);
  this->Internal->SpinCalibrationAlgo->SetMinimumOrientationDifferenceDeg(minimumOrientationDifferenceDeg);
}

//---------------------------------------------------------------------------
int vtkSlicerPivotCalibrationLogic::GetPoseBucketSize()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return 0;
  }
  return algo->GetPoseBucketSize();
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetPoseBucketSize(int bucketSize)
{
  this->Internal->PivotCalibrationAlgo->SetPoseBucketSize(bucketSize);
  this->Internal->SpinCalibrationAlgo->SetPoseBucketSize(bucketSize);
}

//---------------------------------------------------------------------------
int vtkSlicerPivotCalibrationLogic::GetMaximumNumberOfPoseBuckets()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return 0;
  }
  return algo->GetMaximumNumberOfPoseBuckets();
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetMaximumNumberOfPoseBuckets(int bucketSize)
{
  this->Internal->PivotCalibrationAlgo->SetMaximumNumberOfPoseBuckets(bucketSize);
  this->Internal->SpinCalibrationAlgo->SetMaximumNumberOfPoseBuckets(bucketSize);
}

//---------------------------------------------------------------------------
double vtkSlicerPivotCalibrationLogic::GetMaximumPoseBucketError()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return 0.0;
  }
  return algo->GetMaximumPoseBucketError();
}

//---------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetMaximumPoseBucketError(double maximumBucketError)
{
  this->Internal->PivotCalibrationAlgo->SetMaximumPoseBucketError(maximumBucketError);
  this->Internal->SpinCalibrationAlgo->SetMaximumPoseBucketError(maximumBucketError);
}

//-----------------------------------------------------------------------------
double vtkSlicerPivotCalibrationLogic::GetPositionDifferenceThresholdMm()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return 0.0;
  }
  return algo->GetPositionDifferenceThresholdMm();
}

//-----------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetPositionDifferenceThresholdMm(double thresholdMM)
{
  this->Internal->PivotCalibrationAlgo->SetPositionDifferenceThresholdMm(thresholdMM);
  this->Internal->SpinCalibrationAlgo->SetPositionDifferenceThresholdMm(thresholdMM);
}

//-----------------------------------------------------------------------------
double vtkSlicerPivotCalibrationLogic::GetOrientationDifferenceThresholdDegrees()
{
  vtkIGSIOAbstractCalibrationAlgo* algo = this->Internal->GetCurrentCalibrationAlgo();
  if (!algo)
  {
    return 0.0;
  }
  return algo->GetOrientationDifferenceThresholdDegrees();
}

//-----------------------------------------------------------------------------
void vtkSlicerPivotCalibrationLogic::SetOrientationDifferenceThresholdDegrees(double thresholdDegrees)
{
  this->Internal->PivotCalibrationAlgo->SetOrientationDifferenceThresholdDegrees(thresholdDegrees);
  this->Internal->SpinCalibrationAlgo->SetOrientationDifferenceThresholdDegrees(thresholdDegrees);
}
