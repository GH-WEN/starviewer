# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/optimalviewpoint
# L'objectiu �s una aplicaci??:  

FORMS += optimalviewpointinputparametersformbase.ui \
         qoptimalviewpointextensionbase.ui \
         qshowhidewidgetbase.ui 
HEADERS += doublehistogram.h \
           histogram.h \
           optimalviewpoint.h \
           optimalviewpointinputparametersform.h \
           optimalviewpointparameters.h \
           optimalviewpointplane.h \
           optimalviewpointplanehelper.h \
           optimalviewpointviewer.h \
           optimalviewpointvolume.h \
           povspherecloud.h \
           vector3.h \
           vtk4DLinearRegressionGradientEstimator.h \
           vtkInteractorStyleJoystickActorGgg.h \
           vtkInteractorStyleSwitchGgg.h \
           vtkInteractorStyleTrackballActorGgg.h \
           optimalviewpointextensionmediator.h \
           qoptimalviewpointextension.h \
           slicer.h \
           qshowhidewidget.h \
           vtkVolumeRayCastCompositeFunctionObscurances.h \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.h \
           vtkVolumeRayCastCompositeFunctionFx.h \
           vtkVolumeRayCastCompositeFxFunction.h \
           voxelshader.h \
           ambientvoxelshader.h \
           directilluminationvoxelshader.h \
           contourvoxelshader.h \
           obscurancevoxelshader.h \
           saliencyvoxelshader.h \
           hdrcolor.h \
           trilinearinterpolator.h \
           colorbleedingvoxelshader.h \
           obscurancethread.h 
SOURCES += doublehistogram.cpp \
           histogram.cpp \
           optimalviewpoint.cpp \
           optimalviewpointinputparametersform.cpp \
           optimalviewpointparameters.cpp \
           optimalviewpointplane.cpp \
           optimalviewpointplanehelper.cpp \
           optimalviewpointviewer.cpp \
           optimalviewpointvolume.cpp \
           povspherecloud.cpp \
           vector3.cpp \
           vtk4DLinearRegressionGradientEstimator.cxx \
           vtkInteractorStyleJoystickActorGgg.cxx \
           vtkInteractorStyleSwitchGgg.cxx \
           vtkInteractorStyleTrackballActorGgg.cxx \
           optimalviewpointextensionmediator.cpp \
           qoptimalviewpointextension.cpp \
           slicer.cpp \
           qshowhidewidget.cpp \
           vtkVolumeRayCastCompositeFunctionObscurances.cxx \
           vtkVolumeRayCastCompositeFunctionViewpointSaliency.cxx \
           vtkVolumeRayCastCompositeFunctionFx.cxx \
           vtkVolumeRayCastCompositeFxFunction.cxx \
           voxelshader.cpp \
           ambientvoxelshader.cpp \
           directilluminationvoxelshader.cpp \
           contourvoxelshader.cpp \
           obscurancevoxelshader.cpp \
           saliencyvoxelshader.cpp \
           trilinearinterpolator.cpp \
           colorbleedingvoxelshader.cpp \
           obscurancethread.cpp 
RESOURCES += optimalviewpoint.qrc
include(../../basicconfextensions.inc)
