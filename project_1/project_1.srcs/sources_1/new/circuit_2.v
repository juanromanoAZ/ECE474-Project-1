`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/30/2020 03:04:27 PM
// Design Name: 
// Module Name: circuit_2
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


module circuit_2(clk, rst, a, b, c, z, x);
    input clk, rst;
    input [31:0] a, b, c;
    output [31:0] z, x;
    
    wire [31:0] d, e, f, g, h, zwire, xwire;
    wire dLTe, dEQe, u0a, u0b, u1a, u1b;
    
    ADD    #(.WIDTH(32)) add0(a, b, d);
    ADD    #(.WIDTH(32)) add1(a, c, e);
    SUB    #(.WIDTH(32)) sub0(a, b, f);
    COMP   #(.WIDTH(32)) comp0(d, e, u0a, u0b, dEQe);
    COMP   #(.WIDTH(32)) comp1(d, e, u1a, dLTe, u1b);
    MUX2x1 #(.WIDTH(32)) mux0(d, e, dLTe, g);
    MUX2x1 #(.WIDTH(32)) mux1(g, f, dEQe, h);
    SHL    #(.WIDTH(32)) shl0(g, dLTe, xwire);
    SHR    #(.WIDTH(32)) shr0(h, dEQe, zwire);
    REG    #(.WIDTH(32)) reg0(xwire, x, clk, rst);
    REG    #(.WIDTH(32)) reg1(zwire, z, clk, rst);
endmodule
