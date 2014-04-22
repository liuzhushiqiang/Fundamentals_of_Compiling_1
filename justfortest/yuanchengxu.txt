#include <stdio.h>

//calculate the perimeter
double perimeter(double radius, double pi){
	return 2 * pi * radius;
} 

//calculate the factorial
int factorial(int n){
	int result = 1;
	for(int i = 1; i <= n; i++){
		result *= i;
	}
	return result;
}

/*
the main function
*/
void main(){
	perimeter(2, 3.14);
	factorial(10);
	printf("two functions");
}