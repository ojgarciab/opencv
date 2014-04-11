/* ddc.cpp -- Detector de círculos con OpenCV
    Copyright (C) 2010 Oscar García

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Para compilar:
g++ ddc.cpp -o ddc $(pkg-config --cflags --libs opencv)
*/

#include <opencv2/opencv.hpp>
#include <iostream>

int main(int, char**)
{
    // Fotogramas obtenidos del dispositivo de captura y su conversión a grises
    cv::Mat fotograma, grises, canny, histograma, histo_norm;
    // Los círculos se devuelven como un vector de conjuntos de tres valores de coma flotante
    cv::vector<cv::Vec3f> circulos;

    // Abrimos la cámara por defecto
    cv::VideoCapture camara(0);
    // Comprobamos si se ha abierto correctamente
    if(!camara.isOpened())
    {
        return -1;
    }

    // Creamos las ventanas llamadas "Fotogramas" y "Canny" donde mostraremos los resultados
    cv::namedWindow("Canny", 1);
    cv::namedWindow("Fotogramas", 1);

    // El programa finalizará cuando se pulse una tecla (o Control+C en la consola)
    while (cv::waitKey(30) < 0)
    {
        // Obtenemos un nuevo fotograma del dispositivo de captura
        camara >> fotograma;
        // Convertimos el fotograma a escala de grises
        cv::cvtColor(fotograma, grises, CV_BGR2GRAY);
        // Usamos la transformada de Hough para encontrar los círculos
        // http://docs.opencv.org/trunk/modules/imgproc/doc/feature_detection.html#houghcircles
        std::cout << circulos.size() << std::endl;
        cv::HoughCircles(grises, circulos, CV_HOUGH_GRADIENT, 2, fotograma.cols / 2, 150, 150, fotograma.cols / 20);
        // Simulamos el filtro Canny que hace internamente la transformada "trucada" de Hough
        cv::Canny(grises, canny, 75, 150);
        // Por cada circulo detectado lo dibujamos con los parámetros que nos devuelve
        for( size_t i = 0; i < circulos.size(); i++ )
        {
            // El primer elemento del array es la coordenada X, el segundo la Y y el tercero el radio
            cv::Point centro(cvRound(circulos[i][0]), cvRound(circulos[i][1]));
            int radio = cvRound(circulos[i][2]);
            // Dibujamos el círculo en el fotograma original
            cv::circle(fotograma, centro, radio, cv::Scalar(0,0,255), 3, 8, 0);
        }
        // Mostramos el fotograma original con el/los círculos dibujado/s
        cv::imshow("Fotogramas", fotograma);
        // Mostramos la simulación de filtro Canny
        cv::imshow("Canny", canny);
    }
    // Liberamos el dispositivo de captura, no es realmente necesario porque el
    // destructor lo haría por nosotros al finalizar el programa
    camara.release();
    return 0;
}
