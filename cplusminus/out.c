#include <stdio.h> 
 #include <stdlib.h> 
 
 struct Person 
 { 
 const char * name ; 
 int age ; 
 void myMethod ( Person * self ) 
 { 
 printf ( "Called my method! %s" , self -> name ) ; 
 } ; 
 } ; 
 
 // This is a test file 
 int main ( int argc , char * * argv ) 
 { 
 float pi = 3.1425 ; 
 if ( pi == 4 ) 
 { 
 printf ( "Yes" ) ; 
 } 
 
 Person * p = malloc ( sizeof ( Person ) ) ; 
 p -> name = "John" ; 
 p -> age = 49 ; 
 p -> myMethod ( ) ; 
 
 int i = 0 ; 
 i ++ ; 
 
 if ( pi +- 4 ) 
 { 
 printf ( "PI is roughly 4" ) ; 
 } 
 
 printf ( "Hello %s" , "World" ) ; 
 return 0 ; 
 } 
 