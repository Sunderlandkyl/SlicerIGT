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

#ifndef __vtkMRMLPlusRemoteLauncherNode_h
#define __vtkMRMLPlusRemoteLauncherNode_h

// MRML includes
#include <vtkMRMLNode.h>

// PlusRemote includes
#include "vtkSlicerPlusRemoteModuleMRMLExport.h"

class vtkMRMLScene;

/// \ingroup Segmentations
/// \brief Parameter set node for the plus remote launcher widget
///
class VTK_SLICER_PLUSREMOTE_MODULE_MRML_EXPORT vtkMRMLPlusRemoteLauncherNode : public vtkMRMLNode
{

public:
  static vtkMRMLPlusRemoteLauncherNode *New();
  vtkTypeMacro(vtkMRMLPlusRemoteLauncherNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /// Create instance of a GAD node.
  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  /// Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts) VTK_OVERRIDE;

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent) VTK_OVERRIDE;

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node) VTK_OVERRIDE;

  /// Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() VTK_OVERRIDE { return "PlusRemoteLauncher"; }

protected:
  vtkMRMLPlusRemoteLauncherNode();
  ~vtkMRMLPlusRemoteLauncherNode();
  vtkMRMLPlusRemoteLauncherNode(const vtkMRMLPlusRemoteLauncherNode&);
  void operator=(const vtkMRMLPlusRemoteLauncherNode&);
};

#endif // __vtkMRMLPlusRemoteLauncherNode_h
