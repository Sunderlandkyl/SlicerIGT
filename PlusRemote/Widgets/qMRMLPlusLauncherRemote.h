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

#ifndef __qMRMLPlusLauncherRemote_h
#define __qMRMLPlusLauncherRemote_h

// PlusRemote includes
//#include "qSlicerSegmentationsModuleWidgetsExport.h"

// MRMLWidgets includes
#include "qMRMLWidget.h"

/*/// \ingroup SlicerIgt_QtModules_Segmentations_Widgets*/
class Q_SLICER_MODULE_SEGMENTATIONS_WIDGETS_EXPORT qMRMLSegmentsTableView : public qMRMLWidget
{
  Q_OBJECT
  //QVTK_OBJECT

public:
  //Q_PROPERTY(int Mode READ Mode WRITE setMode) //etc

  typedef qMRMLWidget Superclass;
  /// Constructor
  explicit qMRMLSegmentsTableView(QWidget* parent = 0);
  /// Destructor
  virtual ~qMRMLSegmentsTableView();

}
#endif
