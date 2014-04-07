/* dpn.cpp -- Detección de pantalla negra con OpenCV
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
g++ dpn.cpp -o dpn $(pkg-config --cflags --libs opencv)
*/

#include <opencv2/opencv.hpp>

int main(int, char**)
{
    // Fotogramas obtenidos del dispositivo de captura y su conversión a grises
    cv::Mat fotograma, grises, histograma, histo_norm;

    // Abrimos la cámara por defecto
    cv::VideoCapture camara(0);
    // Comprobamos si se ha abierto correctamente
    if(!camara.isOpened())
    {
        return -1;
    }

    // Creamos la ventana llamada "Fotogramas" donde mostraremos el resultado
    cv::namedWindow("Fotogramas",1);

    // El programa finalizará cuando se pulse una tecla (o Control+C en la consola)
    while (cv::waitKey(30) < 0)
    {
        // Obtenemos un nuevo fotograma del dispositivo de captura
        camara >> fotograma;
        // Convertimos el fotograma a escala de grises
        cv::cvtColor(fotograma, grises, CV_BGR2GRAY);
        // Dibujamos un recuadro de 256x128 para contener el histograma
        cv::rectangle(grises, cv::Rect(10, 10, 256, 130), cv::Scalar(255), 1, 8);
        // Sacado del ejemplo de OpenCV:
        // http://docs.opencv.org/doc/tutorials/imgproc/histograms/histogram_calculation/histogram_calculation.html
        // Rango de los valores del canal de escala de grises (0 mínimo, 256 máximo)
        float range[] = { 0, 256 };
        // Como sólo tenemos un canal (segundo parámetro) sólo ponemos un elemento
        const float* histRange = { range };
        // Podríamos "cuantizar" a un reducido número de valores para acelerar el algoritmo
        int histSize = 256;
        // Calculamos el histograma
        cv::calcHist(&grises, 1, 0, cv::Mat(), histograma, 1, &histSize, &histRange);
        // Normalizamos el histograma para dibujarlo en el recuadro
        cv::normalize(histograma, histo_norm, 0, 127, cv::NORM_MINMAX, -1, cv::Mat() );
        int x, i;
        // Dibujamos el histograma
        for (x = 10, i = 0; i < 256; i++, x++)
        {
            cv::line(grises, cv::Point(x, 138 - histo_norm.at<float>(i)), cv::Point(x, 139), cv::Scalar(191), 1, 8);
        }
        // Calculamos la suma de los primeros 20 valores (los tonos más oscuros)
        float porcentaje, suma = 0;
        for (i = 0; i < 20; i++, x++)
        {
            suma += histograma.at<float>(i);
        }
        // Calculamos el porcentaje de pixeles que corresponden al total
        porcentaje = (suma * 100) / (grises.rows * grises.cols);
        // Depuramos el resultado
        std::cout << suma << " (" << porcentaje << "%)" << std::endl;
        // Dibujamos un marco blanco alrededor de la imagen en caso de ser muy oscura
        if (porcentaje > 95) {
            cv::rectangle(grises, cv::Rect(4, 4, grises.cols - 7, grises.rows - 7), cv::Scalar(255), 4, 8);
        }
        // Mostramos el fotograma con el histograma y el aviso dibujado
        cv::imshow("Fotogramas", grises);
    }

    // Liberamos el dispositivo de captura, no es realmente necesario porque el
    // destructor lo haría por nosotros al finalizar el programa
    camara.release();
    return 0;
}
