/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

//  Software Guide : BeginCommandLineArgs
//    INPUTS: {BrainProtonDensitySlice.png}
//    OUTPUTS: {GradientMagnitudeImageFilterOutput.png}
//  Software Guide : EndCommandLineArgs
//
//  Software Guide : BeginLatex
//
//  The magnitude of the image gradient is extensively used in image analysis,
//  mainly to help in the determination of object contours and the
//  separation of homogeneous regions. The
//  \doxygen{GradientMagnitudeImageFilter} computes the magnitude of the
//  image gradient at each pixel location using a simple finite differences
//  approach. For example, in the case of $2D$ the computation is equivalent
//  to convolving the image with masks of type
//
//  \begin{center}
//  \begin{picture}(200,50)
//  \put( 5.0,32.0){\framebox(30.0,15.0){-1}}
//  \put(35.0,32.0){\framebox(30.0,15.0){0}}
//  \put(65.0,32.0){\framebox(30.0,15.0){1}}
//  \put(105.0,17.0){\framebox(20.0,15.0){1}}
//  \put(105.0,32.0){\framebox(20.0,15.0){0}}
//  \put(105.0,47.0){\framebox(20.0,15.0){-1}}
//  \end{picture}
//  \end{center}
//
//  then adding the sum of their squares and computing the square root of the sum.
//
//  This filter will work on images of any dimension thanks to the internal
//  use of \doxygen{NeighborhoodIterator} and \doxygen{NeighborhoodOperator}.
//
//  \index{itk::GradientMagnitudeImageFilter}
//
//  Software Guide : EndLatex


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

//  Software Guide : BeginLatex
//
//  The first step required to use this filter is to include its header file.
//
//  Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "itkTotalVariationDenoisingImageFilter.h"
// Software Guide : EndCodeSnippet

int main( int argc, char * argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  outputImageFile [lambda iterations threads]" << std::endl;
    return EXIT_FAILURE;
    }

  float        lambda  = 0.1; 
  int numberIterations =  30;
  int    numberThreads =   1;
  if( argc > 3 )
    {
    lambda = atof( argv[3] );
    }

  if( argc > 4 )
    {
    numberIterations = atoi( argv[4] );
    }

  if( argc > 5 )
    {
    numberThreads = atoi( argv[5] );
    }

  //  Software Guide : BeginLatex
  //
  //  Types should be chosen for the pixels of the input and output images.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef    float    InputPixelType;
  typedef    float    OutputPixelType;
  const   unsigned int     Dimension = 3;
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The input and output image types can be defined using the pixel types.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;
  // Software Guide : EndCodeSnippet


  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  // read image
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );


  // setup TV
  typedef itk::TotalVariationDenoisingImageFilter<InputImageType,OutputImageType>
    TVFilterType;
  TVFilterType::Pointer tvFilter = TVFilterType::New();
  tvFilter->SetInput(reader->GetOutput());
  tvFilter->SetNumberIterations(numberIterations);
  //tvFilter->SetNumberOfThreads(numberThreads );
  tvFilter->SetLambda(lambda);
  tvFilter->Update();

  //  Software Guide : BeginLatex
  //
  //  If the output of this filter has been connected to other filters in a
  //  pipeline, updating any of the downstream filters will also trigger an
  //  update of this filter. For example, the gradient magnitude filter may be
  //  connected to an image writer.
  //
  //  Software Guide : EndLatex

  //typedef float WritePixelType;
  //typedef itk::Image< WritePixelType, Dimension > WriteImageType;
  //typedef itk::RescaleIntensityImageFilter<
  //             OutputImageType, WriteImageType > RescaleFilterType;

  //RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

  //rescaler->SetOutputMinimum( 0 );
  //rescaler->SetOutputMaximum( 1 );

  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );

  // Software Guide : BeginCodeSnippet
  //rescaler->SetInput( filter->GetOutput() );
  writer->SetInput( tvFilter->GetOutput() );
  writer->Update();
  // Software Guide : EndCodeSnippet



  return EXIT_SUCCESS;
}

