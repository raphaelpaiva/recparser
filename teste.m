int global;

int foo() { return 42; }

void bar() {
        int a;
        
        a = 0;
        
        {
                int b;
                b = foo();
                
                a = a - b;
        }
}

float main(int argc, char[][] argv) {
        int l1, l2, l3;
        
        global = foo();
                
        l1 = 10;
        l2 = 3;
        
        if (global && l2 > global) {
                bar();
                return global;
        }
        
        l3 = l1 + l2*global;
        
        return l3; 
}

