`timescale 1ns / 1ps

module ADD #(parameter WIDTH = 2) (A, B, S, cin, cout);
    input [WIDTH-1:0] A, B;
    input cin;
    output reg cout;
    output reg [WIDTH-1:0] S;
    
    reg [WIDTH-1:0] A5, B5, S5;
        
    always @(A, B, cin) begin
        A5 = {1'b0, A}; 
        B5 = {1'b0, B}; //blocking assignment for intermediate values
        S5 = A5 + B5 + cin;
        
        S <= S5[WIDTH-1:0]; //part selection
        cout <= S5[4];
    end
endmodule