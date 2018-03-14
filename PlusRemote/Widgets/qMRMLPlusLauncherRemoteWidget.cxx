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

// PlusRemote includes
#include "qMRMLPlusLauncherRemoteWidget.h"
#include "ui_qMRMLPlusLauncherRemoteWidget.h"
#include "vtkMRMLPlusRemoteLauncherNode.h"

// QT includes
#include <QtGui>
#include <QDebug>
#include <qfiledialog.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkMRMLAbstractLogic.h>

// MRML includes
#include <vtkMRMLScene.h>

// OpenIGTLinkIF includes
#include <vtkMRMLTextNode.h>
#include <vtkMRMLIGTLConnectorNode.h>
#include <vtkSlicerOpenIGTLinkIFCommand.h>

class qMRMLPlusLauncherRemoteWidgetPrivate : public Ui_qMRMLPlusLauncherRemoteWidget
{
  Q_DECLARE_PUBLIC(qMRMLPlusLauncherRemoteWidget);

protected:
  qMRMLPlusLauncherRemoteWidget* const q_ptr;
public:
  qMRMLPlusLauncherRemoteWidgetPrivate(qMRMLPlusLauncherRemoteWidget& object);
  ~qMRMLPlusLauncherRemoteWidgetPrivate();
  void init();

  int PlusServerLauncherPort;
  vtkMRMLIGTLConnectorNode* LauncherConnectorNode;

  vtkWeakPointer<vtkMRMLPlusRemoteLauncherNode> ParameterSetNode;

};

//-----------------------------------------------------------------------------
qMRMLPlusLauncherRemoteWidgetPrivate::qMRMLPlusLauncherRemoteWidgetPrivate(qMRMLPlusLauncherRemoteWidget& object)
  : q_ptr(&object),
  PlusServerLauncherPort(18944),
  LauncherConnectorNode(nullptr)
{
}

//-----------------------------------------------------------------------------
qMRMLPlusLauncherRemoteWidgetPrivate::~qMRMLPlusLauncherRemoteWidgetPrivate()
{
  Q_Q(qMRMLPlusLauncherRemoteWidget);
}

void qMRMLPlusLauncherRemoteWidgetPrivate::init()
{
  Q_Q(qMRMLPlusLauncherRemoteWidget);
  this->setupUi(q);

  // Launcher connection info 
  QObject::connect(this->launcherConnectCheckBox, SIGNAL(toggled(bool)), q, SLOT(onConnectCheckBoxChanged(bool)));

  // Server control
  QObject::connect(this->loadConfigFileButton, SIGNAL(clicked()), q, SLOT(onLoadConfigFile()));
}

//-----------------------------------------------------------------------------
// qMRMLSegmentEditorWidget methods

//-----------------------------------------------------------------------------
qMRMLPlusLauncherRemoteWidget::qMRMLPlusLauncherRemoteWidget(QWidget* _parent)
  : qMRMLWidget(_parent)
  , d_ptr(new qMRMLPlusLauncherRemoteWidgetPrivate(*this))
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  d->init();
}

//-----------------------------------------------------------------------------
qMRMLPlusLauncherRemoteWidget::~qMRMLPlusLauncherRemoteWidget()
{
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::setMRMLScene(vtkMRMLScene* newScene)
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  if (newScene == this->mrmlScene())
  {
    return;
  }
  
  Superclass::setMRMLScene(newScene);

  d->configFileSelectorComboBox->setMRMLScene(this->mrmlScene());

  // Make connections that depend on the Slicer application
  //QObject::connect(qSlicerApplication::application()->layoutManager(), SIGNAL(layoutChanged(int)), this, SLOT(onLayoutChanged(int)));

  // Update UI
  this->updateWidgetFromMRML();

  // observe close event so can re-add a parameters node if necessary
  //this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::EndCloseEvent, this, SLOT(onMRMLSceneEndCloseEvent()));
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  if (!this->mrmlScene() || this->mrmlScene()->IsClosing())
  {

    d->launcherConnectCheckBox->setDisabled(true);

    return;
  }

}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::onConnectCheckBoxChanged(bool connect)
{
  Q_D(qMRMLPlusLauncherRemoteWidget);

  std::string hostnameString = d->hostnameLineEdit->text().toStdString();
  const char* hostname = hostnameString.c_str();

  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkCommand::ModifiedEvent);

  if (d->LauncherConnectorNode)
  {
    if (strcmp(d->LauncherConnectorNode->GetServerHostname(), hostname) != 0 || d->LauncherConnectorNode->GetServerPort() != d->PlusServerLauncherPort)
    {
      this->setLauncherConnectorNode(nullptr);
    }
  }

  if (!d->LauncherConnectorNode)
  {
    std::vector<vtkMRMLNode*> connectorNodes = std::vector<vtkMRMLNode*>();
    //TODO find static method to get node type name
    this->mrmlScene()->GetNodesByClass("vtkMRMLIGTLConnectorNode", connectorNodes);

    for (std::vector<vtkMRMLNode*>::iterator connectorNodeIt = connectorNodes.begin(); connectorNodeIt != connectorNodes.end(); ++connectorNodeIt)
    {
      vtkMRMLIGTLConnectorNode* connectorNode = vtkMRMLIGTLConnectorNode::SafeDownCast(*connectorNodeIt);
      if (!connectorNode)
      {
        continue;
      }

      if (strcmp(connectorNode->GetServerHostname(), hostname) == 0)
      {
        this->setLauncherConnectorNode(connectorNode);
        break;
      }
    }
  }

  //TODO find static method to get node type name
  if (connect)
  {
    if (!d->LauncherConnectorNode)
    {
      vtkMRMLNode* node = this->mrmlScene()->AddNewNodeByClass("vtkMRMLIGTLConnectorNode", "PlusServerLauncherConnector");
      this->setLauncherConnectorNode(vtkMRMLIGTLConnectorNode::SafeDownCast(node));
    }

    if (d->LauncherConnectorNode->GetState() != d->LauncherConnectorNode->StateOff)
    {
      d->LauncherConnectorNode->Stop();
    }

    d->LauncherConnectorNode->SetServerHostname(hostname);
    d->LauncherConnectorNode->SetServerPort(d->PlusServerLauncherPort);
    d->LauncherConnectorNode->Start();

  }
  else
  {
    if (d->LauncherConnectorNode)
    {
      d->LauncherConnectorNode->Stop();
    }
  }
  
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::setLauncherConnectorNode(vtkMRMLIGTLConnectorNode* connectorNode)
{
  Q_D(qMRMLPlusLauncherRemoteWidget);

  if (!connectorNode)
  {
    qvtkDisconnect(d->LauncherConnectorNode, vtkCommand::ModifiedEvent, this, SLOT(onLauncherConnectorNodeModified()));
    qvtkDisconnect(d->LauncherConnectorNode, vtkMRMLIGTLConnectorNode::ConnectedEvent, this, SLOT(onLauncherConnectorNodeModified()));
    qvtkDisconnect(d->LauncherConnectorNode, vtkMRMLIGTLConnectorNode::DisconnectedEvent, this, SLOT(onLauncherConnectorNodeModified()));
    d->LauncherConnectorNode = nullptr;
    return;
  }

  if (d->LauncherConnectorNode != connectorNode)
  {
    qvtkReconnect(d->LauncherConnectorNode, connectorNode, vtkCommand::ModifiedEvent, this, SLOT(onLauncherConnectorNodeModified()));
    qvtkReconnect(d->LauncherConnectorNode, connectorNode, vtkMRMLIGTLConnectorNode::ConnectedEvent, this, SLOT(onLauncherConnectorNodeModified()));
    qvtkReconnect(d->LauncherConnectorNode, connectorNode, vtkMRMLIGTLConnectorNode::DisconnectedEvent, this, SLOT(onLauncherConnectorNodeModified()));
    d->LauncherConnectorNode = connectorNode;
  }

}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::onLauncherConnectorNodeModified()
{
  Q_D(qMRMLPlusLauncherRemoteWidget);

  int state = d->LauncherConnectorNode->GetState();
  d->launcherConnectCheckBox->setChecked(state != vtkMRMLIGTLConnectorNode::StateOff);
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::onLauncherConnect()
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  QString hostname = d->hostnameLineEdit->text();
  
  // Check connector for existing node
  //this->mrmlScene()->GetNodesByClass()

  //Connect
  //vtkSmartPointer<vtkMRMLIGTLConnectorNode> 
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::onLoadConfigFile()
{
  QString filename = QFileDialog::getOpenFileName(nullptr, "Select config file", "", "Config Files (*.xml);;AllFiles (*)");
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
  {
    return;
  }

  QFileInfo fileInfo(file);
  std::string name = fileInfo.fileName().toStdString();
  QString contents = file.readAll();

  vtkSmartPointer<vtkMRMLTextNode> configFileNode = vtkSmartPointer<vtkMRMLTextNode>::New();
  configFileNode->SetText(contents.toStdString().c_str());
  configFileNode->SetName(name.c_str());

  this->mrmlScene()->AddNode(configFileNode);
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::onLaunchServer()
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  
}

//-----------------------------------------------------------------------------
bool qMRMLPlusLauncherRemoteWidget::advancedOptionsVisible() const
{
  Q_D(const qMRMLPlusLauncherRemoteWidget);
  return d->advancedGroupBox->isVisible();
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::setAdvancedOptionsVisible(bool visible)
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  d->advancedGroupBox->setVisible(visible);
}

//-----------------------------------------------------------------------------
bool qMRMLPlusLauncherRemoteWidget::logVisible() const
{
  Q_D(const qMRMLPlusLauncherRemoteWidget);
  return d->logGroupBox->isVisible();
}

//-----------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::setLogVisible(bool visible)
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  d->logGroupBox->setVisible(visible);
}

//------------------------------------------------------------------------------
vtkMRMLPlusRemoteLauncherNode* qMRMLPlusLauncherRemoteWidget::plusRemoteLauncherNode()const
{
  Q_D(const qMRMLPlusLauncherRemoteWidget);
  return d->ParameterSetNode;
}


//------------------------------------------------------------------------------
void qMRMLPlusLauncherRemoteWidget::setPlusRemoteLauncherNode(vtkMRMLPlusRemoteLauncherNode* newPlusRemoteLauncherNode)
{
  Q_D(qMRMLPlusLauncherRemoteWidget);
  if (d->ParameterSetNode == newPlusRemoteLauncherNode)
  {
    return;
  }

  // Connect modified event on ParameterSetNode to updating the widget
  qvtkReconnect(d->ParameterSetNode, newPlusRemoteLauncherNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()));

  // Set parameter set node
  d->ParameterSetNode = newPlusRemoteLauncherNode;

  if (!d->ParameterSetNode)
  {
    return;
  }

  //this->initializeParameterSetNode();

  // Update UI
  this->updateWidgetFromMRML();
}