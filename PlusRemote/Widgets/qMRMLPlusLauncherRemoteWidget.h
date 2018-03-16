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

#ifndef __qMRMLPlusLauncherRemoteWidget_h
#define __qMRMLPlusLauncherRemoteWidget_h

// PlusRemote includes
#include "qSlicerPlusRemoteModuleWidgetsExport.h"

// MRMLWidgets includes
#include "qMRMLWidget.h"

// CTK includes
#include <ctkVTKObject.h>

class qMRMLPlusLauncherRemoteWidgetPrivate;
class vtkMRMLIGTLConnectorNode;
class vtkMRMLPlusRemoteLauncherNode;

/// \ingroup Slicer_QtModules_PlusRemote
class Q_SLICER_MODULE_PLUSREMOTE_WIDGETS_EXPORT qMRMLPlusLauncherRemoteWidget : public qMRMLWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool logVisible READ logVisible WRITE setLogVisible)
  Q_PROPERTY(bool advancedOptionsVisible READ advancedOptionsVisible WRITE setAdvancedOptionsVisible)

public:
  typedef qMRMLWidget Superclass;
  /// Constructor
  explicit qMRMLPlusLauncherRemoteWidget(QWidget* parent = 0);
  /// Destructor
  virtual ~qMRMLPlusLauncherRemoteWidget();

  /// Get the segment editor parameter set node
  Q_INVOKABLE vtkMRMLPlusRemoteLauncherNode* plusRemoteLauncherNode()const;

  void advancedOptionsVisible(bool);

  /// Show/hide the advanced options section
  bool advancedOptionsVisible() const;
  void setAdvancedOptionsVisible(bool);

  /// Show/hide the log section
  bool logVisible() const;
  void setLogVisible(bool);

public slots:
  /// Set the MRML scene associated with the widget
  virtual void setMRMLScene(vtkMRMLScene* newScene);

  /// Update widget state from the MRML scene
  virtual void updateWidgetFromMRML();

  // TODO
  void onConnectCheckBoxChanged(bool checked);
  virtual void onLauncherConnect();
  virtual void onLoadConfigFile();
  virtual void onLaunchServer();

  void setLauncherConnectorNode(vtkMRMLIGTLConnectorNode*);
  void onLauncherConnectorNodeModified();

  void setPlusRemoteLauncherNode(vtkMRMLPlusRemoteLauncherNode*);

protected:
  QScopedPointer<qMRMLPlusLauncherRemoteWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLPlusLauncherRemoteWidget);
  Q_DISABLE_COPY(qMRMLPlusLauncherRemoteWidget);
};

#endif
