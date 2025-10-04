
int decimal = 123;
int hex = 0x1A2B;
int binary = 0b1010;
uint unsigned = 123U;
long longVal = 123L;
ulong unsignedLong = 123UL;
int withUnderscores = 1_000_000;

double simple = 3.14;
float floatVal = 3.14F;
double doubleVal = 3.14D;
decimal decimalVal = 3.14M;
double exponential = 1.5e10;
double negExp = 1.5E-10;

string simple = "hello";
string withEscape = "hello\nworld";
string verbatim = @"hello";
string verbatimQuote = @"hello""world";
string interpolated = $"hello {name}";
string interpVerbatim = $@"path: {path}";
string empty = "";

char simple = 'a';
char escaped = '\n';
char quote = '\'';

class MyClass {
    public void Method() {
        if (true) {
            foreach (var item in list) {
                return;
            }
        }
    }
}

int myVar;
int _myVar123;
int @class; 

x = a + b;
x++;
x += 5;
bool eq = a == b;
bool neq = a != b;
bool and = a && b;
bool or = a || b;
string result = str ?? "default";
obj?.Method();
Func<int, int> lambda = x => x * 2;
x <<= 2;
x >>= 2;

void Method(int a, int b) {
    int[] array = {1, 2, 3};
    array[0] = 5;
}

#define DEBUG
#if DEBUG
    Console.WriteLine("Debug mode");
#endif

a+++++b;  
x?.ToString() ?? "null"; 
var x = 0xFF_00_FF; 
