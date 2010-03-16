# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/core
# L'objectiu és una biblioteca:
FORMS += qlogviewerbase.ui \
    qcustomwindowleveldialogbase.ui \
    qdicomdumpbase.ui \
    qdicomdumpcthelixwidgetbase.ui \
    qdicomdumpctlocalizerwidgetbase.ui \
    qdicomdumpmrwidgetbase.ui \
    qdicomdumpctwidgetbase.ui \
    qdicomdumpmammographywidgetbase.ui \
    qtransferfunctioneditorbyvaluesbase.ui \
    qtransferfunctionintervaleditorbase.ui \
    qcinecontrollerbase.ui \
    qthickslabwidgetbase.ui \
    q2dviewerwidgetbase.ui
TRANSLATIONS += core_ca_ES.ts \
    core_es_ES.ts \
    core_en_GB.ts
HEADERS += extensionfactory.h \
    genericfactory.h \
    singleton.h \
    genericsingletonfactoryregister.h \
    extensionfactoryregister.h \
    extensionmediator.h \
    extensionmanager.h \
    displayableid.h \
    extensionmediatorfactory.h \
    extensionmediatorfactoryregister.h \
    installextension.h \
    itkImageToVTKImageFilter.h \
    itkVTKImageToImageFilter.h \
    mathtools.h \
    itkQtAdaptor.h \
    harddiskinformation.h \
    logging.h \
    deletedirectory.h \
    volume.h \
    volumerepository.h \
    identifier.h \
    repositorybase.h \
    repository.h \
    q2dviewer.h \
    q3dmprviewer.h \
    q3dviewer.h \
    qviewer.h \
    patient.h \
    series.h \
    study.h \
    output.h \
    q3dorientationmarker.h \
    zoomtool.h \
    qlogviewer.h \
    strokesegmentationmethod.h \
    itkErfcLevelSetFunction.h \
    itkErfcLevelSetImageFilter.h \
    itkVolumeCalculatorImageFilter.h \
    vtkAtamaiPolyDataToImageStencil2.h \
    shutterfilter.h \
    image.h \
    dicomtagreader.h \
    patientfiller.h \
    patientfillerstep.h \
    patientfillerinput.h \
    dicomfileclassifierfillerstep.h \
    imagefillerstep.h \
    presentationstatefillerstep.h \
    extensioncontext.h \
    keyimagenotefillerstep.h \
    temporaldimensionfillerstep.h \
    createinformationmodelobject.h \
    dicomimagereader.h \
    dicomimagereaderitkgdcm.h \
    dicomimagereadervtk.h \
    dicomimagereaderdcmtk.h \
    patientbrowsermenu.h \
    patientbrowsermenubasicitem.h \
    patientbrowsermenuextendeditem.h \
    patientbrowsermenulist.h \
    colorpalette.h \
    qcustomwindowleveldialog.h \
    qwindowlevelcombobox.h \
    mhdfileclassifierstep.h \
    orderimagesfillerstep.h \
    toolregistry.h \
    toolmanager.h \
    toolconfiguration.h \
    tool.h \
    toolproxy.h \
    tooldata.h \
    referencelinestool.h \
    referencelinestooldata.h \
    imageplane.h \
    slicingtool.h \
    translatetool.h \
    voxelinformationtool.h \
    windowleveltool.h \
    seedtool.h \
    magictool.h \
    rotate3dtool.h \
    screenshottool.h \
    synchronizetool.h \
    synchronizetooldata.h \
    windowlevelpresetstool.h \
    drawer.h \
    drawerprimitive.h \
    drawerpolygon.h \
    drawerline.h \
    drawertext.h \
    drawerpolyline.h \
    polylineroitool.h \
    polylinetemporalroitool.h \
    polylinetemporalroitooldata.h \
    distancetool.h \
    editortool.h \
    editortooldata.h \
    windowlevelpresetstooldata.h \
    qviewercinecontroller.h \
    qdicomdumpcthelixwidget.h \
    qdicomdumpctlocalizerwidget.h \
    qdicomdump.h \
    qdicomdumpmrwidget.h \
    qdicomdumpctwidget.h \
    qdicomdumpmammographywidget.h \
    qcinecontroller.h \
    hoverpoints.h \
    qcolorspinbox.h \
    qtransferfunctioneditorbyvalues.h \
    qtransferfunctionintervaleditor.h \
    shadewidget.h \
    transferfunction.h \
    transferfunctionio.h \
    qtransferfunctioneditor.h \
    qtransferfunctioneditorbygradient.h \
    accumulator.h \
    averageaccumulator.h \
    minimumaccumulator.h \
    maximumaccumulator.h \
    vtkProjectionImageFilter.h \
    slicingkeyboardtool.h \
    qthickslabwidget.h \
    erasertool.h \
    patientitem.h \
    patientitemmodel.h \
    drawerpoint.h \
    seedtooldata.h \
    cursor3dtool.h \
    cursor3dtooldata.h \
    angletool.h \
    drawercrosshair.h \
    starviewerapplication.h \
    viewerslayout.h \
    q2dviewerwidget.h \
    hangingprotocol.h \
    hangingprotocollayout.h \
    hangingprotocolsloader.h \
    hangingprotocolsrepository.h \
    hangingprotocolmanager.h \
    hangingprotocolmask.h \
    hangingprotocolimageset.h \
    hangingprotocoldisplayset.h \
    hangingprotocolxmlreader.h \
    ambientvoxelshader.h \
    contourvoxelshader.h \
    directilluminationvoxelshader.h \
    voxelshader.h \
    hdrcolor.h \
    trilinearinterpolator.h \
    vector3.h \
    vtkVolumeRayCastVoxelShaderCompositeFunction.h \
    sphereuniformpointcloudgenerator.h \
    obscurancemainthread.h \
    obscurancethread.h \
    obscurancevoxelshader.h \
    vtk4DLinearRegressionGradientEstimator.h \
    combiningvoxelshader.h \
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction.h \
    obscurance.h \
    viewpointgenerator.h \
    thumbnailcreator.h \
    nonclosedangletool.h \
    abortrendercommand.h \
    roitool.h \
    statswatcher.h \
    clippingplanestool.h \
    representationslayer.h \
    toolrepresentation.h \
    settings.h \
    settingsregistry.h \
    settingsparser.h \
    defaultsettings.h \
    coresettings.h \
    settingsaccesslevelfilereader.h \
    glew.h \
    glxew.h \
    wglew.h \
    shortcuts.h \
    shortcutmanager.h \
    volumebuilder.h \
    volumebuilderfromcaptures.h \
    dicomattribute.h \
    dicomvalueattribute.h \
    dicomsequenceattribute.h \
    dicomsequenceitem.h \
    dicomtag.h \
    dicomwriter.h \
    dicomwriterdcmtk.h \
    dicomfilegenerator.h \
    dicomdictionary.h \
    dicomimagefilegenerator.h \
    copydirectory.h 
SOURCES += extensionmediator.cpp \
    extensionmanager.cpp \
    displayableid.cpp \
    mathtools.cpp \
    harddiskinformation.cpp \
    deletedirectory.cpp \
    volume.cpp \
    volumerepository.cpp \
    identifier.cpp \
    repositorybase.cpp \
    repository.cpp \
    q2dviewer.cpp \
    q3dmprviewer.cpp \
    q3dviewer.cpp \
    qviewer.cpp \
    patient.cpp \
    series.cpp \
    study.cpp \
    output.cpp \
    q3dorientationmarker.cpp \
    zoomtool.cpp \
    editortool.cpp \
    editortooldata.cpp \
    qlogviewer.cpp \
    strokesegmentationmethod.cpp \
    itkErfcLevelSetFunction.cpp \
    itkErfcLevelSetImageFilter.cpp \
    itkVolumeCalculatorImageFilter.cpp \
    vtkAtamaiPolyDataToImageStencil2.cxx \
    shutterfilter.cpp \
    image.cpp \
    dicomtagreader.cpp \
    patientfiller.cpp \
    patientfillerstep.cpp \
    patientfillerinput.cpp \
    dicomfileclassifierfillerstep.cpp \
    imagefillerstep.cpp \
    presentationstatefillerstep.cpp \
    extensioncontext.cpp \
    keyimagenotefillerstep.cpp \
    temporaldimensionfillerstep.cpp \
    createinformationmodelobject.cpp \
    dicomimagereader.cpp \
    dicomimagereaderitkgdcm.cpp \
    dicomimagereadervtk.cpp \
    dicomimagereaderdcmtk.cpp \
    patientbrowsermenu.cpp \
    patientbrowsermenubasicitem.cpp \
    patientbrowsermenuextendeditem.cpp \
    patientbrowsermenulist.cpp \
    colorpalette.cpp \
    qcustomwindowleveldialog.cpp \
    qwindowlevelcombobox.cpp \
    mhdfileclassifierstep.cpp \
    orderimagesfillerstep.cpp \
    toolregistry.cpp \
    toolmanager.cpp \
    toolconfiguration.cpp \
    tool.cpp \
    toolproxy.cpp \
    tooldata.cpp \
    referencelinestool.cpp \
    referencelinestooldata.cpp \
    imageplane.cpp \
    translatetool.cpp \
    slicingtool.cpp \
    voxelinformationtool.cpp \
    windowleveltool.cpp \
    seedtool.cpp \
    magictool.cpp \
    rotate3dtool.cpp \
    screenshottool.cpp \
    synchronizetool.cpp \
    synchronizetooldata.cpp \
    windowlevelpresetstool.cpp \
    drawer.cpp \
    drawerprimitive.cpp \
    drawerpolygon.cpp \
    drawerline.cpp \
    drawertext.cpp \
    drawerpolyline.cpp \
    polylineroitool.cpp \
    polylinetemporalroitool.cpp \
    polylinetemporalroitooldata.cpp \
    distancetool.cpp \
    windowlevelpresetstooldata.cpp \
    qviewercinecontroller.cpp \
    qdicomdump.cpp \
    qdicomdumpcthelixwidget.cpp \
    qdicomdumpctlocalizerwidget.cpp \
    qdicomdumpmrwidget.cpp \
    qdicomdumpctwidget.cpp \
    qdicomdumpmammographywidget.cpp \
    qcinecontroller.cpp \
    hoverpoints.cpp \
    qcolorspinbox.cpp \
    qtransferfunctioneditorbyvalues.cpp \
    qtransferfunctionintervaleditor.cpp \
    shadewidget.cpp \
    transferfunction.cpp \
    transferfunctionio.cpp \
    qtransferfunctioneditor.cpp \
    qtransferfunctioneditorbygradient.cpp \
    accumulator.cpp \
    vtkProjectionImageFilter.cxx \
    slicingkeyboardtool.cpp \
    qthickslabwidget.cpp \
    erasertool.cpp \
    patientitem.cpp \
    patientitemmodel.cpp \
    drawerpoint.cpp \
    seedtooldata.cpp \
    cursor3dtool.cpp \
    cursor3dtooldata.cpp \
    angletool.cpp \
    drawercrosshair.cpp \
    viewerslayout.cpp \
    q2dviewerwidget.cpp \
    hangingprotocol.cpp \
    hangingprotocollayout.cpp \
    hangingprotocolsloader.cpp \
    hangingprotocolsrepository.cpp \
    hangingprotocolmanager.cpp \
    hangingprotocolmask.cpp \
    hangingprotocolimageset.cpp \
    hangingprotocoldisplayset.cpp \
    hangingprotocolxmlreader.cpp \
    ambientvoxelshader.cpp \
    contourvoxelshader.cpp \
    directilluminationvoxelshader.cpp \
    voxelshader.cpp \
    trilinearinterpolator.cpp \
    vtkVolumeRayCastVoxelShaderCompositeFunction.cxx \
    sphereuniformpointcloudgenerator.cpp \
    obscurancemainthread.cpp \
    obscurancethread.cpp \
    obscurancevoxelshader.cpp \
    vtk4DLinearRegressionGradientEstimator.cxx \
    combiningvoxelshader.cpp \
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction.cxx \
    obscurance.cpp \
    viewpointgenerator.cpp \
    thumbnailcreator.cpp \
    nonclosedangletool.cpp \
    abortrendercommand.cpp \
    roitool.cpp \
    statswatcher.cpp \
    clippingplanestool.cpp \
    representationslayer.cpp \
    toolrepresentation.cpp \
    settings.cpp \
    settingsregistry.cpp \
    settingsparser.cpp \
    defaultsettings.cpp \
    coresettings.cpp \
    settingsaccesslevelfilereader.cpp \
    glew.c \
    shortcuts.cpp \
    shortcutmanager.cpp \
    volumebuilder.cpp \
    volumebuilderfromcaptures.cpp \
    dicomattribute.cpp \
    dicomvalueattribute.cpp \
    dicomsequenceattribute.cpp \
    dicomsequenceitem.cpp \
    dicomtag.cpp \
    dicomwriter.cpp \
    dicomwriterdcmtk.cpp \
    dicomfilegenerator.cpp \
    dicomimagefilegenerator.cpp \
    copydirectory.cpp
TEMPLATE = lib
DESTDIR = ./
include(../corelibsconfiguration.inc)
include(../itk.inc)
include(../vtk.inc)
include(../dcmtk.inc)
include(../log4cxx.inc)
include(../compilationtype.inc)
include(../translations.inc)
QT += xml \
    network

