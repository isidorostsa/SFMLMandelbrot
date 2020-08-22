#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>

#include "definitions.h"

//  DOUBLE VERSION 

struct Point{
    double x, y;

    Point(double x, double y) : x(x), y(y) {}
};


struct ViewBox{
    double x1, y1, x2, y2;
    Point center;

    ViewBox(double x1, double y1, double x2, double y2)
        : x1(x1), y1(y1), x2(x2), y2(y2),
            center((x1 + x2) / 2.0, (y1 + y2) / 2.0) {}

    Point Pixel_to_Point(size_t w, size_t h) const{
        return Point(W_Pixel_to_Point(w), H_Pixel_to_Point(h));
    }

    double W_Pixel_to_Point(size_t w) const{
        return x1 + ((double)w / WIDTH) * (x2 - x1);  
    }

    double H_Pixel_to_Point(size_t h) const{
        return y1 + ((double)h / WIDTH) * (y2 - y1);  
    }

    void Update(double new_x1, double new_y1, double new_x2, double new_y2){
        x1 = new_x1;
        y1 = new_y1;
        x2 = new_x2;
        y2 = new_y2;
    }

};


sf::Color colorPicker(size_t n);


size_t Check(double re, double im);


int* CheckArrea(const ViewBox& view);


inline void UpdateImage(const int* iterations, sf::RenderWindow& window);



int main(){

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML");
    window.setFramerateLimit(2);
    window.clear();

    ViewBox view(-2, 2, 2, -2);

    bool shouldUpdateImg = true;
    
    sf::Clock clock;

    while(window.isOpen()){

        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosition2f(view.W_Pixel_to_Point(mousePosition.x), view.H_Pixel_to_Point(mousePosition.y));

        sf::Event event;

        while(window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            
            case sf::Event::MouseButtonPressed:
                view.Update((view.x1 + mousePosition2f.x) / 2, (view.y1 + mousePosition2f.y) / 2,
                             (view.x2 + mousePosition2f.x) / 2, (view.y2 + mousePosition2f.y) / 2);
                shouldUpdateImg = true; 
                break;
            
            case sf::Event::MouseButtonReleased:
                break;

            default:
                break;
            }

        }

        if(shouldUpdateImg){
            window.clear();

            std::cout << "frame!" << std::endl;
            clock.restart();

            int* iterations = CheckArrea(view);
            UpdateImage(iterations, window);
            std::cout << "Time it took to render last frame: " << clock.restart().asSeconds() << std::endl;

            window.display();
            shouldUpdateImg = false;
        }
    }
}


sf::Color colorPicker(long double n){
    return colorPicker((size_t)(n * 1536));
}


sf::Color colorPicker(double n){
    return colorPicker((size_t)(n * 1536));
}


sf::Color colorPicker(size_t n){

    if( 0 <= n < 256 )
        return sf::Color(n % 256 , 0, 255);

    if( 256 <= n < 512 )
        return sf::Color(255 - (n % 256) , 255, 0);

    if( 512 <= n < 768 )
        return sf::Color(0, 255, n % 256);

    if( 768 <= n < 1024 )
        return sf::Color(255 , 0, 255 - (n % 256));

    if( 1024 <= n < 1280 )
        return sf::Color(255, n, 0);

    if( 1280 <= n < 1536 )
        return sf::Color(0 , 255 - (n % 256), 255);

    else
        return sf::Color::White;
}


size_t Check(double re, double im){

    double c_re = 0.0;
    double c_im = 0.0;

    for(size_t iter = 0; iter < MAX_ITER; iter++)
    {
        double c_re_new = c_re*c_re - c_im*c_im + re;
        double c_im_new = 2*c_re*c_im + im;

        if(c_re_new*c_re_new + c_im_new*c_im_new >= 4)
        {
            return iter;
        }

        c_re = c_re_new;
        c_im = c_im_new;
    }

    return MAX_ITER;
}


int* CheckArrea(const ViewBox& view){
    
    int* iterations = new int[WIDTH * HEIGHT];

#pragma omp parallel for collapse(4)
    for(size_t w = 0; w < WIDTH; w++)
    {
        for(size_t h = 0; h < HEIGHT; h++)
        {
            iterations[index(w, h)] = Check(view.W_Pixel_to_Point(w), view.H_Pixel_to_Point(h));
        }
    }

    return iterations;
}


inline void UpdateImage(const int* iterations, sf::RenderWindow& window){
    for(size_t w = 0; w < WIDTH; w++)
    {
        for(size_t h = 0; h < HEIGHT; h++)
        {
            int iters = iterations[index(w, h)];
            
            sf::VertexArray point(sf::Points, 1);
            point[0].position = sf::Vector2f(w, h);
            point[0].color = sf::Color(255, 255, 255, normalize(iters, MAX_ITER) * 255);
            
            window.draw(point);
        }
    }
}


