`timescale 1ns / 1ps

module REG #(parameter WIDTH = 2) (I, Q, clk, rst);
    input [WIDTH-1:0] I;
    input clk, rst;
    output reg [WIDTH-1:0] Q;
    
    
    always @(posedge clk) begin
        if(rst == 1) //synchronous reset
            Q <= {WIDTH{1'b0}};
        else
            Q <= I;
    end

endmodule