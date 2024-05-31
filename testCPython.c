#include <stdio.h>

int square(int i) {
	return i * i;
}

void test() {
    printf("Hello, World !\n");
}




struct shape_interface {
  // Function pointers for the interface functions
  double (*area)(void*);
  double (*perimeter)(void*);
};  

// Define a struct for a circle
struct circle {
  // Data fields for the circle
  double radius;
  // Pointer to the interface struct
  struct shape_interface* shape;
};



// Define the area function for the circle
double circle_area(void* obj) {
  // Cast the void pointer to a circle pointer
  struct circle* c = (struct circle*) obj;
  // Return the area of the circle
  return 3.14 * c->radius * c->radius;
}



// Define the perimeter function for the circle
double circle_perimeter(void* obj) {
  // Cast the void pointer to a circle pointer
  struct circle* c = (struct circle*) obj;
  // Return the perimeter of the circle
  return 2 * 3.14 * c->radius;
}



// Create a global instance of the interface struct
struct shape_interface circle_interface = {
  // Assign the interface functions to the function pointers
  circle_area,
  circle_perimeter
};



// Create a function to initialize a circle
struct circle* circle_new(double radius) {
  // Allocate memory for the circle
  struct circle* c = malloc(sizeof(struct circle));
  // Set the data fields of the circle
  c->radius = radius;
  // Set the pointer to the interface struct
  c->shape = &circle_interface;
  // Return the circle
  return c;
}        