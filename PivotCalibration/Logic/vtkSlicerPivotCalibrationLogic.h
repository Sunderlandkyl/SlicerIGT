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

// .NAME vtkSlicerPivotCalibrationLogic
// .SECTION Description
#ifndef __vtkSlicerPivotCalibrationLogic_h
#define __vtkSlicerPivotCalibrationLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkMath.h>

// MRML includes
#include "vtkMRMLLinearTransformNode.h"

// STD includes
#include <cstdlib>

// VNL includes
#include "vnl/vnl_matrix.h"

#include "vtkSlicerPivotCalibrationModuleLogicExport.h"


/// \ingroup Slicer_QtModules_ExtensionTemplate
/// Module for calibrating a tracked pointer/stylus device.
/// Includes pivot calibration for determining tip position and spin calibration for determining orientation.
/// Shaft direction (i.e. flipping) is automatically determined from both spin calibration and pivot calibration (otherwise the flipping is arbitrary).
/// Shaft direction (i.e. flipping) assumes that the tool marker/sensor is on the same side as the tool base.
/// Pivot and spin calibrations may be performed in arbitrary order.
class VTK_SLICER_PIVOTCALIBRATION_MODULE_LOGIC_EXPORT vtkSlicerPivotCalibrationLogic :
  public vtkSlicerModuleLogic
{
public:

  enum Events
  {
    InputTransformAdded = vtkCommand::UserEvent + 173,
    CalibrationCompleteEvent
  };

  enum CalibrationMode
  {
    PIVOT_CALIBRATION,
    SPIN_CALIBRATION
  };

  static vtkSlicerPivotCalibrationLogic* New();
  vtkTypeMacro(vtkSlicerPivotCalibrationLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // Clears all previously acquired tool transforms.
  // Call this before start adding transforms.
  void ClearToolToReferenceMatrices();

  // Add a tool transforms automatically by observing transform changes
  vtkGetMacro(RecordingState, bool);
  vtkSetMacro(RecordingState, bool);
  void SetAndObserveTransformNode(vtkMRMLLinearTransformNode*);

  // Add a single tool transform manually
  void AddToolToReferenceMatrix(vtkMatrix4x4*);

  // Computes calibration results.
  // By default, automatically flips the shaft direction to be consistent with the needle orientation protocol.
  // Returns with false on failure
  bool ComputePivotCalibration(bool autoOrient = true);

  // Computes calibration results.
  // By default, automatically flips the shaft direction to be consistent with the needle orientation protocol.
  // Optionally, snaps the rotation to be a 90 degree rotation about one of the coordinate axes.
  // Returns with false on failure
  bool ComputeSpinCalibration(bool snapRotation = false, bool autoOrient = true); // Note: The neede orientation protocol assumes that the shaft of the tool lies along the negative z-axis

  // Flip the direction of the shaft axis
  void FlipShaftDirection();

  // Get calibration results
  void GetToolTipToToolTranslation(vtkMatrix4x4*);
  void GetToolTipToToolRotation(vtkMatrix4x4*);
  void GetToolTipToToolMatrix(vtkMatrix4x4*);
  void SetToolTipToToolMatrix(vtkMatrix4x4*);
  vtkGetMacro(PivotRMSE, double);
  vtkGetMacro(SpinRMSE, double);

  // Returns human-readable description of the error occurred (non-empty if ComputePivotCalibration returns with failure)
  std::string GetErrorText();

  /// TODO: name
  int GetNumberOfPoses();

  /// 
  int GetErrorCode();

  vtkGetMacro(CalibrationMode, int);
  vtkSetMacro(CalibrationMode, int);

  vtkGetMacro(AutoCalibrationStopWhenComplete, bool);
  vtkSetMacro(AutoCalibrationStopWhenComplete, bool);
  vtkBooleanMacro(AutoCalibrationStopWhenComplete, bool);

  vtkGetMacro(AutoCalibrationEnabled, bool);
  vtkSetMacro(AutoCalibrationEnabled, bool);
  vtkBooleanMacro(AutoCalibrationEnabled, bool);

  vtkGetMacro(AutoCalibrationNumberOfPoints, int);
  vtkSetMacro(AutoCalibrationNumberOfPoints, int);

  vtkGetMacro(AutoCalibrationStopTargetError, double);
  vtkSetMacro(AutoCalibrationStopTargetError, double);

  // TODO
  int GetPoseBucketSize();
  void SetPoseBucketSize(int bucketSize);

  double GetMinimumOrientationDifferenceDeg();
  void SetMinimumOrientationDifferenceDeg(double);

  int GetMaximumNumberOfPoseBuckets();
  void SetMaximumNumberOfPoseBuckets(int);

  double GetMaximumPoseBucketError();
  void SetMaximumPoseBucketError(double);

  double GetPositionDifferenceThresholdMm();
  void SetPositionDifferenceThresholdMm(double);

  double GetOrientationDifferenceThresholdDegrees();
  void SetOrientationDifferenceThresholdDegrees(double);

protected:
  vtkSlicerPivotCalibrationLogic();
  virtual ~vtkSlicerPivotCalibrationLogic();
  
  void ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData ) override;

  static void ProcessPivotCalibrationAlgorithmEvents(vtkObject* caller, unsigned long event, void* clientData, void* callData);

  bool ComputeAutoCalibration();

  vtkSetMacro(PivotRMSE, double);
  vtkSetMacro(SpinRMSE, double);

  class vtkInternal;
  vtkInternal* Internal;

private:

  vtkSlicerPivotCalibrationLogic(const vtkSlicerPivotCalibrationLogic&); // Not implemented
  void operator=(const vtkSlicerPivotCalibrationLogic&);               // Not implemented

  // Calibration inputs
  vtkMRMLLinearTransformNode* ObservedTransformNode;
  bool RecordingState;

  int CalibrationMode{ PIVOT_CALIBRATION };

  // Calibration results
  vtkMatrix4x4* ToolTipToToolMatrix;
  double PivotRMSE;
  double SpinRMSE;

  double AutoCalibrationStopTargetError{ 3.0 };
  int AutoCalibrationNumberOfPoints{ 50 };
  bool AutoCalibrationStopWhenComplete{ false };
  bool AutoCalibrationEnabled{ false };
};

#endif
