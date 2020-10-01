`timescale 1ns / 1ps

module ADD #(parameter WIDTH = 2) (A, B, S);
    input [WIDTH-1:0] A, B;
    output reg [WIDTH-1:0] S;
            
    always @(A, B) begin
        S <= A + B;
    end
endmodule