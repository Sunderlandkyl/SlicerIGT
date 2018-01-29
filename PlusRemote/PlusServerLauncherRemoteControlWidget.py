import os.path, datetime
from __main__ import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging


class PlusServerLauncherRemoteControlWidget():
  def __init__(self, plusRemote):
  
    self.plusRemote = plusRemote
    
    ##self.plusRemoteModuleDirectoryPath = self.plusRemote.plusRemoteModuleDirectoryPath
    #self.connectIcon = qt.QIcon(self.plusRemoteModuleDirectoryPath+'/Resources/Icons/icon_Connect.png')
  
    self.layout = qt.QFormLayout()
    
    self.selectConfigFileButton = qt.QPushButton()
    #self.selectConfigFileButton.setIcon(self)
    self.selectConfigFileButton.text = "Load"
    self.selectConfigFileButton.setEnabled(True)
    #self.selectConfigFileButton.setToolTip("If clicked, start server")
    #self.selectConfigFileButton.setSizePolicy(qt.QSizePolicy.Fixed, qt.QSizePolicy.Expanding)
    self.layout.addRow("Load Config File:", self.selectConfigFileButton)
  
    self.configFileSelector = slicer.qMRMLNodeComboBox()
    self.configFileSelector.nodeTypes = ( ("vtkMRMLTextNode"), "" ) #TODO: add correct type of node and/or implement
    self.configFileSelector.selectNodeUponCreation = True
    self.configFileSelector.addEnabled = False
    self.configFileSelector.removeEnabled = True
    self.configFileSelector.noneEnabled = False
    self.configFileSelector.showHidden = False
    self.configFileSelector.showChildNodeTypes = False
    self.configFileSelector.setMRMLScene( slicer.mrmlScene )
    self.configFileSelector.setToolTip( "Pick config file" )
    self.layout.addRow("Config File: ", self.configFileSelector)
  
    #Move to the same row, use grid and box layouts
    serverLauncherControlsLayout = qt.QHBoxLayout()
    self.layout.addRow(serverLauncherControlsLayout)
  
    self.serverRunning = False
    self.waitingForResponse = False
    self.startStopServerButton = qt.QPushButton()
    #self.startStopServerButton.setIcon(self.connectIcon)
    self.startStopServerButton.setEnabled(True)
    self.startStopServerButton.setToolTip("If clicked, start server")
    self.startStopServerButton.setSizePolicy(qt.QSizePolicy.Expanding, qt.QSizePolicy.Expanding)
    serverLauncherControlsLayout.addWidget(self.startStopServerButton)
  
    self.serverStatus = qt.QMessageBox()
    self.serverStatus.setIcon(qt.QMessageBox.Information)
    self.serverStatus.setToolTip("Server status")
    self.serverStatus.setStandardButtons(qt.QMessageBox.NoButton)
    self.serverStatus.setEnabled(False)
    serverLauncherControlsLayout.addWidget(self.serverStatus)
  
    self.LOG_LEVEL_ERROR=1
    self.LOG_LEVEL_WARNING=2
    self.LOG_LEVEL_INFO=3
    self.LOG_LEVEL_DEBUG=4
    self.LOG_LEVEL_TRACE=5
  
    self.logLevelComboBox=qt.QComboBox()
    self.logLevelComboBox.setMaximumSize(200, 1000)
    self.logLevelComboBox.addItem(str("Error"), self.LOG_LEVEL_ERROR)
    self.logLevelComboBox.addItem(str("Warning"), self.LOG_LEVEL_WARNING)
    self.logLevelComboBox.addItem(str("Info"), self.LOG_LEVEL_INFO)
    self.logLevelComboBox.addItem(str("Debug"), self.LOG_LEVEL_DEBUG)
    self.logLevelComboBox.addItem(str("Trace"), self.LOG_LEVEL_TRACE)
    self.logLevelComboBox.setCurrentIndex(2)
    self.layout.addRow("Server Log Level:", self.logLevelComboBox)
    
  
    self.serverLogCollapsibleGroupBox = ctk.ctkCollapsibleGroupBox()
    self.serverLogCollapsibleGroupBox.setTitle("Server log")
    self.serverLogCollapsibleGroupBox.collapsed = True
    serverLogLayout = qt.QVBoxLayout()
    self.serverLogCollapsibleGroupBox.setLayout(serverLogLayout)
    self.layout.addRow(self.serverLogCollapsibleGroupBox)
  
    self.serverLogBox = qt.QPlainTextEdit()
    self.serverLogBox.setReadOnly(True)
    serverLogLayout.addWidget(self.serverLogBox)
    self.layout.addRow(serverLogLayout)
  
    self.updatePlusServerRemoteControlButtons()
    
    self.startStopServerButton.connect('clicked(bool)', self.onStartStopServerButton)
    self.selectConfigFileButton.connect('clicked(bool)', self.onSelectConfigFileButton)


  def updatePlusServerRemoteControlButtons(self):
    #TODO: better way to manage button state?
    self.logLevelComboBox.enabled = False
    self.selectConfigFileButton.enabled = False
    self.configFileSelector.enabled = False
    self.serverStatus.enabled = True
    if (self.waitingForResponse):
      self.startStopServerButton.enabled = False
      self.serverStatus.enabled = False
      if (self.serverRunning):
        self.startStopServerButton.text = "  Stopping..."
      else:
        self.startStopServerButton.text = "  Launching..."
    else:
      self.startStopServerButton.enabled = True
      if (self.serverRunning):
        self.startStopServerButton.text = "  Stop Server"
      else:
        self.startStopServerButton.text = "  Launch Server"
        self.serverStatus.enabled = False
        self.logLevelComboBox.enabled = True
        self.selectConfigFileButton.enabled = True
        self.configFileSelector.enabled = True

  def onSelectConfigFileButton(self):
    filename = qt.QFileDialog.getOpenFileName(slicer.util.mainWindow(), "Select Config File", "", "Config Files (*.xml);;AllFiles (*)")
    head, tail = os.path.split(filename)

    file = open(filename, 'r')
    text = file.read()
    file.close()

    textNode = slicer.vtkMRMLTextNode()
    textNode.SetName(tail)
    textNode.SetText(text)
    slicer.mrmlScene.AddNode(textNode)


  def onStartStopServerButton(self):
    if (self.serverRunning):
      self.StopServer()
    else:
      self.StartServer()


  def StartServer(self):
    if (self.plusRemote.connectorNode == None):
      return

    configFileNode = self.configFileSelector.currentNode()
    if (configFileNode == None):
      return

    configFileString = configFileNode.GetText()
    c = ConfigFileMessage(configFileString, self.logLevelComboBox.currentData)
    commandDevice = slicer.modules.openigtlinkremote.logic().SendCommand(self.plusRemote.connectorNode.GetID(), 'CMD_1', 'StartServer', c.getMessage())

    #responseEvent = 119002
    #commandDevice.AddObserver(responseEvent, self.onStartServerResponse)
    #self.waitingForResponse = True
    #self.updatePlusServerRemoteControlButtons()

  @vtk.calldata_type(vtk.VTK_OBJECT)
  def onCommandEvent(self, obj, event, callData):
    commandContents = slicer.modules.openigtlinkremote.logic().GetDeviceContents(callData)

    rootElement = vtk.vtkXMLUtilities.ReadElementFromString(commandContents)
    
    messageElement = rootElement.FindNestedElementWithName("Message")
    if (messageElement != None):
      self.onMessageReceived(messageElement)
      
    startServerElement = rootElement.FindNestedElementWithName("StartServer")
    if (startServerElement != None):
      self.onStartServer(startServerElement)
      
    stopServerElement = rootElement.FindNestedElementWithName("StopServer")
    if (stopServerElement != None):
      self.onStopServer(stopServerElement)

  # React to server start message. Response or command.
  def onStartServer(self, startServerElement):
    if (startServerElement.GetAttribute("Success") != None):
      success = startServerElement.GetAttribute("Success")
      if (success.lower() == "true"):
        self.serverRunning = True
      else:
        self.serverRunning = True
      
    #TODO: should check if success
    self.waitingForResponse = False
    self.updatePlusServerRemoteControlButtons()
  
  # React to server stop message. Response or command.  
  def onStopServer(self, stopServerElement):
    if (stopServerElement.GetAttribute("Success") != None):
      success = stopServerElement.GetAttribute("Success")
      if (success.lower() == "true"):
        self.serverRunning = False
      else:
        self.serverRunning = True
        
    #TODO: should check if success
    self.waitingForResponse = False
    self.updatePlusServerRemoteControlButtons()

  def onMessageReceived(self, messageElement):
    if (messageElement != None and messageElement.GetAttribute("Text") != None):
      messageText = messageElement.GetAttribute("Text")
      self.serverLogBox.appendPlainText(messageText.replace('###NEWLINE###', '\n')[:-1])

  def StopServer(self):
    commandString = '''
<Command>
  <StopServer/>
</Command>
'''
    commandDevice = slicer.modules.openigtlinkremote.logic().SendCommand(self.plusRemote.connectorNode.GetID(), 'CMD_2', 'StopServer', commandString)
    responseEvent = 119002
    commandDevice.AddObserver(responseEvent, self.onStopServerResponse)
    self.waitingForResponse = True
    self.updatePlusServerRemoteControlButtons()

  def onStopServerResponse(self, responseDevice, event):
    #TODO: should check if failure
    self.waitingForResponse = False
    self.serverRunning = False
    self.updatePlusServerRemoteControlButtons()

  def onConnectorNodeSelected(self):

    # Force initial update
    if self.plusRemote.connectorNode:

      # Add observers for connect/disconnect events
      #events = [[slicer.vtkMRMLIGTLConnectorNode.ConnectedEvent, self.onConnectorNodeConnected], [slicer.vtkMRMLIGTLConnectorNode.DisconnectedEvent, self.onConnectorNodeDisconnected]]
      #for tagEventHandler in events:
      #  connectorNodeObserverTag = self.connectorNode.AddObserver(tagEventHandler[0], tagEventHandler[1])
      #  self.connectorNodeObserverTagList.append(connectorNodeObserverTag)
      
      commandEvent = 119001
      self.plusRemote.connectorNode.AddObserver(commandEvent, self.onCommandEvent)
      self.plusRemote.connectorNodeObserverTagList.append(commandEvent)
      
      commandResponseEvent = 119002
      self.plusRemote.connectorNode.AddObserver(commandResponseEvent, self.onCommandEvent)
      self.plusRemote.connectorNodeObserverTagList.append(commandResponseEvent)
      self.serverLogBox.setPlainText("")
    
class ConfigFileMessage():
  def __init__(self, configFileString, verbosity):
   self.configFileString = configFileString
   self.verbosity = verbosity

  def getMessage(self):
    message = '''
<Command>
  <StartServer>
    {0}
    <LogLevel Level="{1}"/>
  </StartServer>
</Command>
'''
    message = message.format(self.configFileString, self.verbosity)
    return message