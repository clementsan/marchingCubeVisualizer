#include <iostream>
#include "itkImageToVTKImageFilter.h"
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkCastImageFilter.h>
#include <vtkProperty.h>

//#include <vtkImageMarchingCubes.h>
#include "vtkMyMarchingCubes.h"
//#include <vtkMarchingCubes.h>

#include <vtkPolyDataMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkExtractEdges.h>
#include <vtkPolyDataConnectivityFilter.h>

#include <vtkPolyDataWriter.h>

#include "argio.h"

int main( int argc, const char ** argv ) {

  if (argc<3) {
    std::cout << "Usage: ";
    std::cout << "marchingCubeVisualizer infile.sfx outfile.vtk [threshval] [illthreshval]" << std::endl;
    return -1;
  }

  //typedef itk::Image< short , 3 > ImageTypeLM;
  typedef itk::Image< double , 3 > ImageTypeLM;
  typedef itk::ImageFileReader< ImageTypeLM > ReaderType;
  typedef itk::Image< double , 3 > ImageType;

  float dThreshVal = 0.5;
  if (argc>=4) {
    sscanf( argv[3], "%f", &dThreshVal );

  }

  std::cout << "Computing marching cube surface at " << dThreshVal << std::endl;

  float dIllThreshVal = -0.75;
  if (argc>=5) {
    sscanf( argv[4], "%f", &dIllThreshVal );
  }

  std::cout << "Illegal threshold value " << dIllThreshVal << std::endl;

  ReaderType::Pointer readerOrig = ReaderType::New();
  //vtkImageMarchingCubes *marchingCubes = vtkImageMarchingCubes::New();
  //vtkMarchingCubes *marchingCubes = vtkMarchingCubes::New();
  vtkMyMarchingCubes *marchingCubes = vtkMyMarchingCubes::New();

  marchingCubes->SetIllegalValueThreshold( -0.75 );

  std::cout << "filename = " << argv[1] << std::endl;

  readerOrig->SetFileName( argv[1] );

  typedef itk::CastImageFilter< ImageTypeLM, ImageType > CastFilterType; 
  CastFilterType::Pointer reader = CastFilterType::New();

  reader->SetInput( readerOrig->GetOutput() );
  reader->Update();

  typedef itk::ImageToVTKImageFilter< ImageType > ConnectorType;

  ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput( reader->GetOutput() );
  connector->Update();

  marchingCubes->SetInput( connector->GetOutput() );
  marchingCubes->SetNumberOfContours( 1 );
  marchingCubes->SetValue( 0, dThreshVal );
  
  marchingCubes->Update();

  vtkPolyDataWriter *pdWriter = vtkPolyDataWriter::New();
  pdWriter->SetInput( marchingCubes->GetOutput() );
  pdWriter->SetFileName( argv[2] );
  pdWriter->Write();

  int iNumberOfPoints = marchingCubes->GetOutput()->GetPoints()->GetNumberOfPoints();
  int iNumberOfCells = marchingCubes->GetOutput()->GetNumberOfCells();

  // determine number of edges

  vtkExtractEdges *extractEdges = vtkExtractEdges::New();
  extractEdges->SetInput( marchingCubes->GetOutput() );
  extractEdges->Update();

  int iNumberOfEdges = extractEdges->GetOutput()->GetNumberOfLines();
  int iEulerCharacteristic = iNumberOfPoints-iNumberOfEdges+iNumberOfCells;

  std::cout << "Number of vertices = " << iNumberOfPoints << std::endl;
  std::cout << "Number of triangles = " << iNumberOfCells << std::endl;
  std::cout << "Euler number = " << iEulerCharacteristic << std::endl;

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
      
  mapper->SetInput( marchingCubes->GetOutput() );
      
  vtkActor *actor = vtkActor::New();
  actor->SetMapper( mapper );
  actor->GetProperty()->SetOpacity( 0.6 );

  vtkRenderer *ren = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
      
  renWin->SetWindowName( argv[1] );
      
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
      
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(style);
      
  ren->AddActor( actor );
      
  renWin->AddRenderer( ren );
  renWin->SetSize( 1000, 1000 );
  ren->SetBackground(1,1,1); // Background color white
  iren->SetRenderWindow( renWin );
  renWin->Render();
  iren->Start();

  return 0; 

}
