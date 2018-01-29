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

// PlusConfigFile MRML includes
#include "vtkMRMLTextStorageNode.h"

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLTextStorageNode);

//----------------------------------------------------------------------------
vtkMRMLTextStorageNode::vtkMRMLTextStorageNode()
{
  this->AddFileName("E:/d/p/PR/PlusLibData/ConfigFiles/PlusDeviceSet_Server_MmfColorVideoCapture.xml");
}

//----------------------------------------------------------------------------
vtkMRMLTextStorageNode::~vtkMRMLTextStorageNode()
{
}

//----------------------------------------------------------------------------
std::string vtkMRMLTextStorageNode::UpdateFileList(vtkMRMLNode *refNode, int move)
{
  return "";
}

//----------------------------------------------------------------------------
void vtkMRMLTextStorageNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLTextStorageNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts); // This will take care of referenced nodes

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLTextStorageNode::Copy(vtkMRMLNode *anode)
{
  vtkMRMLTextStorageNode* srcNode = vtkMRMLTextStorageNode::SafeDownCast(anode);
  if (srcNode == NULL)
  {
    vtkErrorMacro("vtkMRMLTextStorageNode::Copy failed: expected intput with vtkMRMLTextStorageNode type");
    return;
  }
  Superclass::Copy(anode); // This will take care of referenced nodes

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLTextStorageNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os, indent); // This will take care of referenced nodes
}

//----------------------------------------------------------------------------
bool vtkMRMLTextStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLTextStorageNode");
}

//----------------------------------------------------------------------------
bool vtkMRMLTextStorageNode::CanWriteFromReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLTextStorageNode");
}