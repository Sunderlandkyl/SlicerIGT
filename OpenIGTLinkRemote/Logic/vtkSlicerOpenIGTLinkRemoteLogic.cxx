#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkMRMLIGTLQueryNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLTextNode.h"
#include "vtkSlicerOpenIGTLinkCommand.h"
#include "vtkSlicerOpenIGTLinkIFLogic.h"
#include "vtkSlicerOpenIGTLinkRemoteLogic.h"

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkXMLDataElement.h>
#include <vtkXMLUtilities.h>

// Share a single command counter across all possible logic instances.
int vtkSlicerOpenIGTLinkRemoteLogic::CommandCounter = 0;

//----------------------------------------------------------------------------

class vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal
{
public:
  vtkInternal();

  vtkSlicerOpenIGTLinkIFLogic* IFLogic;

  // Command query nodes and corresponding command objects.
  // After a command is responded the query node is kept in the scene to avoid the overhead of removing and re-adding command query nodes.
  struct CommandInfo
  {
    igtlio::CommandDevicePointer CommandDevice;
    vtkSmartPointer<vtkSlicerOpenIGTLinkCommand> Command;
  };
  std::vector<CommandInfo> Commands;
};

vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::vtkInternal()
: IFLogic(NULL)
{
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerOpenIGTLinkRemoteLogic);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkSlicerOpenIGTLinkRemoteLogic::vtkSlicerOpenIGTLinkRemoteLogic()
{
  this->Internal = new vtkInternal;
}

//----------------------------------------------------------------------------
vtkSlicerOpenIGTLinkRemoteLogic::~vtkSlicerOpenIGTLinkRemoteLogic()
{
  for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator it = this->Internal->Commands.begin();
    it != this->Internal->Commands.end(); ++it)
  {
    //this->DeleteCommandQueryNode(it->CommandQueryNode);
  }
  this->Internal->Commands.clear();
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::SetIFLogic(vtkSlicerOpenIGTLinkIFLogic* ifLogic)
{
  this->Internal->IFLogic = ifLogic;
}

//----------------------------------------------------------------------------
vtkMRMLIGTLQueryNode* vtkSlicerOpenIGTLinkRemoteLogic::CreateCommandQueryNode()
{
  if (this->GetMRMLScene() == NULL)
  {
    vtkErrorMacro("vtkSlicerOpenIGTLinkRemoteLogic::CreateCommandQueryNode failed: invalid scene");
    return NULL;
  }
  vtkSmartPointer< vtkMRMLIGTLQueryNode > commandQueryNode = vtkSmartPointer< vtkMRMLIGTLQueryNode >::New();
  commandQueryNode->SetHideFromEditors(true);
  commandQueryNode->SetSaveWithScene(false);
  this->GetMRMLScene()->AddNode(commandQueryNode);

  // Also create the response node. It allows the node to be hidden and excluded from saving with the scene.
  vtkSmartPointer< vtkMRMLTextNode > responseNode = vtkSmartPointer< vtkMRMLTextNode >::New();
  responseNode->SetHideFromEditors(true);
  responseNode->SetSaveWithScene(false);
  this->GetMRMLScene()->AddNode(responseNode);

  commandQueryNode->SetResponseDataNodeID(responseNode->GetID());

  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLIGTLQueryNode::ResponseEvent);
  vtkObserveMRMLNodeEventsMacro(commandQueryNode, events.GetPointer());

  return commandQueryNode;
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::DeleteCommandQueryNode(vtkMRMLIGTLQueryNode* commandQueryNode)
{
  vtkUnObserveMRMLNodeMacro(commandQueryNode);

  if (this->GetMRMLScene() == NULL)
  {
    vtkErrorMacro("vtkSlicerOpenIGTLinkRemoteLogic::DiscardCommand failed: invalid scene");
    return;
  }

  if (commandQueryNode == NULL)
  {
    vtkErrorMacro("vtkSlicerOpenIGTLinkRemoteLogic::DiscardCommand failed: invalid queryNode");
    return;
  }

  // Delete response node
  vtkMRMLNode* responseNode = commandQueryNode->GetResponseDataNode();
  if (responseNode != NULL)
  {
    this->GetMRMLScene()->RemoveNode(responseNode);
  }

  // Delete query node
  this->GetMRMLScene()->RemoveNode(commandQueryNode);
}

//----------------------------------------------------------------------------
vtkMRMLIGTLQueryNode* vtkSlicerOpenIGTLinkRemoteLogic::GetCommandQueryNode(vtkSlicerOpenIGTLinkCommand* command)
{
  //  // If we find an unassigned command query node then use that
  //  for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator it=this->Internal->Commands.begin();
  //    it!=this->Internal->Commands.end(); ++it)
  //  {
  //    if (it->Command.GetPointer()==NULL)
  //    {
  //      // found a command that may be usable
  //      if (it->CommandQueryNode->GetID()==NULL ||
  //        this->GetMRMLScene()->GetNodeByID(it->CommandQueryNode->GetID())!=it->CommandQueryNode.GetPointer() )
  //      {
  //        // invalid query node, cannot use it (probably the query node has been removed from the scene)
  //        continue;
  //      }
  //      it->Command=command;
  //      return it->CommandQueryNode;
  //    }
  //  }
  //  // No unassigned command query nodes, so create a new one
  //  vtkMRMLIGTLQueryNode* commandQueryNode = CreateCommandQueryNode();
  //  vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo commandInfo;
  //  commandInfo.CommandQueryNode = commandQueryNode;
  //  commandInfo.Command = command;
  //  this->Internal->Commands.push_back(commandInfo);
  //  return commandInfo.CommandQueryNode;
  return NULL;
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::ReleaseCommandQueryNode(vtkMRMLIGTLQueryNode* commandQueryNode)
{
  //  for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator it=this->Internal->Commands.begin();
  //    it!=this->Internal->Commands.end(); ++it)
  //  {
  //    if (it->CommandQueryNode.GetPointer()==commandQueryNode)
  //    {
  //      it->Command = NULL;
  //    }
  //  }
  std::cout << "TEST" << std::endl;
}

//----------------------------------------------------------------------------
bool vtkSlicerOpenIGTLinkRemoteLogic::SendCommand(vtkSlicerOpenIGTLinkCommand* command, const char* connectorNodeId)
{
  
  if (command == NULL)
  {
    vtkErrorMacro("SendCommand failed: command is invalid");
    return false;
  }
  if (this->GetMRMLScene() == NULL)
  {
    vtkErrorMacro("MRML Scene is invalid");
    return false;
  }
  vtkMRMLIGTLConnectorNode* connectorNode = vtkMRMLIGTLConnectorNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(connectorNodeId));
  if (connectorNode == NULL)
  {
    vtkErrorMacro("SendCommand could not cast MRML node to IGTLConnectorNode.");
    return false;
  }
  if (command->GetStatus() == vtkSlicerOpenIGTLinkCommand::CommandWaiting)
  {
    vtkWarningMacro("vtkSlicerOpenIGTLinkRemoteLogic::SendCommand failed: command is already in progress");
    return false;
  }

  //// Create a unique Id for this command message.
  //// The logic may only be used from the main thread, so there is no need
  //// for making the counter increment thread-safe.
  (this->CommandCounter)++;
  std::stringstream commandIdStream;
  commandIdStream << "CMD_" << this->CommandCounter;
  std::string commandId = commandIdStream.str();
  //std::string commandDeviceName = "CMD";

  command->SetResponseText(NULL); // sets status to FAIL
  command->SetStatus(vtkSlicerOpenIGTLinkCommand::CommandWaiting);
  command->SetID(commandId.c_str()); 

  // TODO: ASYNCHRONOUS might be better, but having trouble getting the correct responses
  //igtlio::CommandDevicePointer commandDevice = connectorNode->SendCommand("CMD", command->GetCommandName(), command->GetCommandText(), igtlio::ASYNCHRONOUS, command->GetCommandTimeoutSec());
  igtlio::CommandDevicePointer commandDevice = connectorNode->SendCommand("CMD", command->GetCommandName(), command->GetCommandText(), igtlio::BLOCKING, command->GetCommandTimeoutSec());
  commandDevice->AddObserver(igtlio::Device::CommandResponseReceivedEvent, this, &vtkSlicerOpenIGTLinkRemoteLogic::ProcessDeviceEvents);

  vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo commandInfo;
  commandInfo.CommandDevice = commandDevice;
  commandInfo.Command = command;
  this->Internal->Commands.push_back(commandInfo);

  commandDevice->InvokeEvent(igtlio::Device::CommandResponseReceivedEvent);
  commandDevice->RemoveObserver(igtlio::Device::CommandResponseReceivedEvent);

  //std::cout << "_____SENDCMND_____" << std::endl;
  //std::cout << commandDevice->GetDeviceName() << std::endl;
  //std::cout << commandDevice->GetDeviceType() << std::endl;
  //std::cout << commandDevice->GetContent().id << std::endl;
  //std::cout << commandDevice->GetContent().name << std::endl;
  //std::cout << commandDevice->GetContent().content << std::endl;
  //std::cout << "__________________" << std::endl;





  return true;
}

std::string vtkSlicerOpenIGTLinkRemoteLogic::GetDeviceContents(vtkObject* device)
{
  igtlio::CommandDevicePointer command = reinterpret_cast<igtlio::CommandDevice*>(device);
  return command->GetContent().content;
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::ProcessDeviceEvents(vtkObject* caller, unsigned long event, void * callData)
{
  if (igtlio::Device::CommandResponseReceivedEvent)
  {
    igtlio::CommandDevice* commandDevice = dynamic_cast<igtlio::CommandDevice*>(caller);
    if (!commandDevice)
    {
      vtkErrorMacro("Could not find command device.");
      return;
    }

    // Find the SlicerOpenIGTLinkRemote command that corresponds with the current response device
    vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo commandInfo;
    for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator commandIt = this->Internal->Commands.begin();
      commandIt != this->Internal->Commands.end(); ++commandIt)
    {
      if (commandDevice == (*commandIt).CommandDevice.GetPointer())
      {
        commandInfo = (*commandIt);
        break;
      }
    }

    vtkSmartPointer<vtkSlicerOpenIGTLinkCommand> command = commandInfo.Command;
    if (!command)
    {
      vtkErrorMacro("Could not find a corresponsing command.");
      return;
    }

    igtlio::CommandDevice* responseDevice = commandDevice->GetResponseFromCommandID(commandDevice->GetContent().id);

    if (!responseDevice)
    {
      responseDevice = commandDevice;
    }

    // Do response things.
    //std::cout << "_____RESPONSE_____" << std::endl;
    //std::cout << responseDevice->GetDeviceName() << std::endl;
    //std::cout << responseDevice->GetDeviceType() << std::endl;
    //std::cout << responseDevice->GetContent().id << std::endl;
    //std::cout << responseDevice->GetContent().name << std::endl;
    //std::cout << responseDevice->GetContent().content << std::endl;
    //std::cout << "__________________" << std::endl;

    command->SetStatus(vtkSlicerOpenIGTLinkCommand::CommandSuccess);
    command->SetResponseText(responseDevice->GetContent().content.c_str());
    command->InvokeEvent(vtkSlicerOpenIGTLinkCommand::CommandCompletedEvent);

  }
  else
  {
    //igtlio::CommandDevice* commandDevice = dynamic_cast<igtlio::CommandDevice*>(caller);
    //if (!commandDevice)
    //{
    //  vtkErrorMacro("Could not find command device.");
    //  return;
    //}
    //commandDevice->RemoveObserver(igtlio::Device::CommandResponseReceivedEvent);
  }
}

//----------------------------------------------------------------------------
bool vtkSlicerOpenIGTLinkRemoteLogic::CancelCommand(vtkSlicerOpenIGTLinkCommand* command)
{
  if (command == NULL)
  {
    vtkErrorMacro("vtkSlicerOpenIGTLinkRemoteLogic::CancelCommand failed: invalid input command");
    return false;
  }
  //for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator it=this->Internal->Commands.begin();
  //  it!=this->Internal->Commands.end(); ++it)
  //{
  //  if (it->Command.GetPointer()==command)
  //  {
  //    // Clean up command query node
  //    if (it->CommandQueryNode->GetConnectorNode())
  //    {
  //      it->CommandQueryNode->GetConnectorNode()->CancelQuery(it->CommandQueryNode);
  //    }
  //    it->CommandQueryNode->SetQueryStatus(vtkMRMLIGTLQueryNode::STATUS_NOT_DEFINED);
  //    // Clean up command node
  //    it->Command->SetStatus(vtkSlicerOpenIGTLinkCommand::CommandCancelled);
  //    // Notify caller
  //    it->Command->InvokeEvent(vtkSlicerOpenIGTLinkCommand::CommandCompletedEvent, it->CommandQueryNode);
  //    ReleaseCommandQueryNode(it->CommandQueryNode);
  //    return true;
  //  }
  //}
  return false;
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::RegisterNodes()
{
  if (this->GetMRMLScene() == NULL)
  {
    vtkErrorMacro("Scene is invalid");
    return;
  }
}

//----------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::UpdateFromMRMLScene()
{
  if (this->GetMRMLScene() == 0)
  {
    vtkErrorMacro("Scene is invalid");
    return;
  }

  // Cancel and remove those commands for that the corresponding query node is deleted from the scene
  std::vector< vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo* > commandInfoToBeDeleted;
  for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator it = this->Internal->Commands.begin();
    it != this->Internal->Commands.end(); ++it)
  {
    //if (it->CommandQueryNode->GetID()!=NULL
    //  && this->GetMRMLScene()->GetNodeByID(it->CommandQueryNode->GetID())==it->CommandQueryNode.GetPointer())
    //{
    //  // command query node is still in the scene
    //  continue;
    //}
    // command query node is not in the scene anymore, cancel the command
    this->CancelCommand(it->Command);
    commandInfoToBeDeleted.push_back(&(*it));
  }

  // Delete all cancelled items
  // Do it in a separate loop from the checking, as cancelling a command might have the side effect of adding/removing commands
  for (std::vector< vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo* >::iterator itemToDeleteIt = commandInfoToBeDeleted.begin();
    itemToDeleteIt != commandInfoToBeDeleted.end(); ++itemToDeleteIt)
  {
    // find and delete command item
    for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator it = this->Internal->Commands.begin();
      it != this->Internal->Commands.end(); ++it)
    {
      if ((*itemToDeleteIt) == &(*it))
      {
        // found it
        this->Internal->Commands.erase(it);
        break;
      }
    }
  }

}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  if (node && !node->IsA("vtkMRMLIGTLQueryNode"))
  {
    return;
  }
  // A vtkMRMLIGTLQueryNode has been deleted
  // Make sure that our query nodes are still valid
  UpdateFromMRMLScene();
}

//---------------------------------------------------------------------------
void vtkSlicerOpenIGTLinkRemoteLogic::ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void * callData)
{
  if (event == vtkMRMLIGTLQueryNode::ResponseEvent)
  {
    // We obtain command and query node before making any change to the command object
    // because the query node, command object, and the command queue may change  in response
    // to command object changes (e.g., a module may poll the status of a command
    // and submit new commands when it detects that a command is completed)
    vtkSmartPointer<vtkMRMLIGTLQueryNode> commandQueryNode;
    vtkSmartPointer<vtkSlicerOpenIGTLinkCommand> command;
    for (std::vector<vtkSlicerOpenIGTLinkRemoteLogic::vtkInternal::CommandInfo>::iterator it = this->Internal->Commands.begin();
      it != this->Internal->Commands.end(); ++it)
    {
      //if (it->CommandQueryNode.GetPointer()==caller)
      //{
      //  // found the command
      //  commandQueryNode = it->CommandQueryNode;
      //  command = it->Command;
      //  break;
      //}
    }
    if (commandQueryNode.GetPointer() == NULL || command.GetPointer() == NULL)
    {
      // command not found
      vtkErrorMacro("vtkSlicerOpenIGTLinkRemoteLogic::ProcessMRMLNodesEvents error: vtkMRMLIGTLQueryNode::ResponseEvent received for an invalid command");
      return;
    }
    // Update the command node with the results in the query node
    //ReleaseCommandQueryNode(commandQueryNode);
    if (commandQueryNode->GetQueryStatus() == vtkMRMLIGTLQueryNode::STATUS_EXPIRED)
    {
      command->SetStatus(vtkSlicerOpenIGTLinkCommand::CommandExpired);
    }
    else
    {
      vtkMRMLTextNode* responseNode = vtkMRMLTextNode::SafeDownCast(commandQueryNode->GetResponseDataNode());
      if (responseNode)
      {
        // this sets status, too; because the success code is specified in the response text
        command->SetResponseText(responseNode->GetText());
      }
      else
      {
        command->SetStatus(vtkSlicerOpenIGTLinkCommand::CommandFail);
      }
    }
    command->InvokeEvent(vtkSlicerOpenIGTLinkCommand::CommandCompletedEvent, commandQueryNode);
    // We must return and do nothing else after InvokeEvent because the notified modules
    // might have changed the command list.
    return;
  }

  // By default let the parent class to handle events
  this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
}
