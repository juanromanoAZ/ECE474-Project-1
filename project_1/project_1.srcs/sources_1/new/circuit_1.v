`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/30/2020 12:30:24 PM
// Design Name: 
// Module Name: circuit_1
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module circuit_1(clk, rst, a, b, c, z, x);
    input clk, rst;
    input [7:0] a, b, c;
    output [7:0] z;
    output [15:0] x;
    
    wire [7:0] d, e, uEQ, uLT;
    wire [15:0] f, g, xwire;
    
    ADD    #(.WIDTH(8)) add0(a, b, d);
    ADD    #(.WIDTH(8)) add1(a, c, e);
    COMP   #(.WIDTH(8)) comp0(d, e, g, uLT, uEQ);
    MUX2x1 #(.WIDTH(8)) mux0(e, d, g, z);
    MUL    #(.WIDTH(16)) mul0(a, c, f);
    SUB    #(.WIDTH(16)) sub0(f, d, xwire);
    REG    #(.WIDTH(16)) reg0(xwire, x, clk, rst);
    
endmodule
