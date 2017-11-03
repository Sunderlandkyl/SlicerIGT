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
#include "vtkMRMLPlusConfigFileNode.h"

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLPlusConfigFileNode);

//----------------------------------------------------------------------------
vtkMRMLPlusConfigFileNode::vtkMRMLPlusConfigFileNode()
{
  this->AddFileName("E:\d\p\PR\PlusLibData\ConfigFiles\PlusDeviceSet_Server_MmfColorVideoCapture.xml");
}

//----------------------------------------------------------------------------
vtkMRMLPlusConfigFileNode::~vtkMRMLPlusConfigFileNode()
{
}

//----------------------------------------------------------------------------
std::string vtkMRMLPlusConfigFileNode::UpdateFileList(vtkMRMLNode *refNode, int move)
{
  return "";
}

//----------------------------------------------------------------------------
void vtkMRMLPlusConfigFileNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLPlusConfigFileNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts); // This will take care of referenced nodes

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLPlusConfigFileNode::Copy(vtkMRMLNode *anode)
{
  vtkMRMLPlusConfigFileNode* srcNode = vtkMRMLPlusConfigFileNode::SafeDownCast(anode);
  if (srcNode == NULL)
  {
    vtkErrorMacro("vtkMRMLPlusConfigFileNode::Copy failed: expected intput with vtkMRMLPlusConfigFileNode type");
    return;
  }
  Superclass::Copy(anode); // This will take care of referenced nodes

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLPlusConfigFileNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os, indent); // This will take care of referenced nodes
}

//----------------------------------------------------------------------------
bool vtkMRMLPlusConfigFileNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLPlusConfigFileNode");
}

//----------------------------------------------------------------------------
bool vtkMRMLPlusConfigFileNode::CanWriteFromReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLPlusConfigFileNode");
}