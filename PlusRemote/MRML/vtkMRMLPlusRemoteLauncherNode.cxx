/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// Segmentations MRML includes
#include "vtkMRMLPlusRemoteLauncherNode.h"

#include "vtkMRMLSegmentationNode.h"
#include "vtkOrientedImageDataResample.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
//static const char* SEGMENTATION_REFERENCE_ROLE = "segmentationRef";
//static const char* MASTER_VOLUME_REFERENCE_ROLE = "masterVolumeRef";

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLPlusRemoteLauncherNode);

//----------------------------------------------------------------------------
vtkMRMLPlusRemoteLauncherNode::vtkMRMLPlusRemoteLauncherNode()
{
}

//----------------------------------------------------------------------------
vtkMRMLPlusRemoteLauncherNode::~vtkMRMLPlusRemoteLauncherNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLPlusRemoteLauncherNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  //of << " selectedSegmentID=\"" << (this->SelectedSegmentID?this->SelectedSegmentID:"") << "\"";
  //of << " activeEffectName=\"" << (this->ActiveEffectName?this->ActiveEffectName:"") << "\"";
  //of << " maskMode=\"" << vtkMRMLPlusRemoteLauncherNode::ConvertMaskModeToString(this->MaskMode) << "\"";
  //of << " maskSegmentID=\"" << (this->MaskSegmentID?this->MaskSegmentID:"") << "\"";
  //of << " masterVolumeIntensityMask=\"" << (this->MasterVolumeIntensityMask ? "true" : "false") << "\"";
  //of << " masterVolumeIntensityMaskRange=\"" << this->MasterVolumeIntensityMaskRange[0] << " " << this->MasterVolumeIntensityMaskRange[1] << "\"";
  //of << " overwriteMode=\"" << vtkMRMLPlusRemoteLauncherNode::ConvertOverwriteModeToString(this->OverwriteMode) << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLPlusRemoteLauncherNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName = NULL;
  const char* attValue = NULL;

  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);

    /*if (!strcmp(attName, "selectedSegmentID"))
      {
      this->SetSelectedSegmentID(attValue);
      }
    else if (!strcmp(attName, "activeEffectName"))
      {
      this->SetActiveEffectName(attValue);
      }
    else if (!strcmp(attName, "maskMode"))
      {
      this->SetMaskMode(vtkMRMLPlusRemoteLauncherNode::ConvertMaskModeFromString(attValue));
      }
    else if (!strcmp(attName, "maskSegmentID"))
      {
      this->SetMaskSegmentID(attValue);
      }
    else if (!strcmp(attName, "masterVolumeIntensityMask"))
      {
      this->SetMasterVolumeIntensityMask(!strcmp(attValue,"true"));
      }
    else if (!strcmp(attName, "masterVolumeIntensityMaskRange"))
      {
      std::stringstream ss;
      ss << attValue;
      double range[2]={0};
      ss >> range[0];
      ss >> range[1];
      this->SetMasterVolumeIntensityMaskRange(range);
      }
    else if (!strcmp(attName, "overwriteMode"))
      {
      this->SetOverwriteMode(vtkMRMLPlusRemoteLauncherNode::ConvertOverwriteModeFromString(attValue));
      }*/
    }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLPlusRemoteLauncherNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  this->DisableModifiedEventOn();

  vtkMRMLPlusRemoteLauncherNode* otherNode = vtkMRMLPlusRemoteLauncherNode::SafeDownCast(anode);

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLPlusRemoteLauncherNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
//void vtkMRMLPlusRemoteLauncherNode::SetAndObserveSegmentationNode(vtkMRMLSegmentationNode* node)
//{
//  this->SetNodeReferenceID(SEGMENTATION_REFERENCE_ROLE, (node ? node->GetID() : NULL));
//}
