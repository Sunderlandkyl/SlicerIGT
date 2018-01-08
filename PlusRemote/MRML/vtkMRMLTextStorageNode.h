/*==============================================================================

Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
Queen's University, Kingston, ON, Canada. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This file was originally developed by Kyle Sunderland, PerkLab, Queen's University
and was supported through the Applied Cancer Research Unit program of Cancer Care
Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

#ifndef __vtkMRMLTextStorageNode_h
#define __vtkMRMLTextStorageNode_h

#include <vtkMRMLStorageNode.h>

// PlusRemote includes
#include "vtkSlicerPlusRemoteModuleMRMLExport.h"

class VTK_SLICER_PLUSREMOTE_MODULE_MRML_EXPORT vtkMRMLTextStorageNode : public vtkMRMLStorageNode
{
public:

  //----------------------------------------------------------------
  // MRML methods

  static vtkMRMLTextStorageNode* New();
  vtkTypeMacro(vtkMRMLTextStorageNode, vtkMRMLStorageNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  ///
  /// Read node attributes from XML file
  virtual void ReadXMLAttributes(const char** atts) VTK_OVERRIDE;

  ///
  /// Do a temp write to update the file list in this storage node with all
  /// file names that are written when write out the ref node
  /// If move is 1, return the directory that contains the written files and
  /// only the written files, for use in a move instead of a double
  /// write. Otherwise return an empty string.
  std::string UpdateFileList(vtkMRMLNode *refNode, int move = 0);

  ///
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent) VTK_OVERRIDE;

  ///
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node) VTK_OVERRIDE;

  ///
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() VTK_OVERRIDE{return "PlusConfigFile";}

  ///
  /// Method to propagate events generated in mrml
  //virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) VTK_OVERRIDE;

  /// Return true if the reference node is supported by the storage node
  virtual bool CanReadInReferenceNode(vtkMRMLNode* refNode) VTK_OVERRIDE;
  virtual bool CanWriteFromReferenceNode(vtkMRMLNode* refNode) VTK_OVERRIDE;

  ///
  /// Configure the storage node for data exchange. This is an
  /// opportunity to optimize the storage node's settings, for
  /// instance to turn off compression.
  //virtual void ConfigureForDataExchange() VTK_OVERRIDE;

protected:

  // Constructor/destructor methods
  vtkMRMLTextStorageNode();
  ~vtkMRMLTextStorageNode();
  vtkMRMLTextStorageNode(const vtkMRMLTextStorageNode&);
  void operator=(const vtkMRMLTextStorageNode&);

  /// Initialize all the supported write file types
  //virtual void InitializeSupportedWriteFileTypes() VTK_OVERRIDE;

};

#endif
