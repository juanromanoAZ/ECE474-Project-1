`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/30/2020 03:05:12 PM
// Design Name: 
// Module Name: circuit_4
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


module circuit_4(a, b, c, z, x, clk, rst);
    input clk, rst;
    input [63:0] a, b, c;
    output [31:0] z, x;
    
    wire [63:0] d, e, f, g, h, xrin, zrin;
    wire dLTe, dEQe, u0a, u0b, u1a, u1b;
    wire [63:0] greg, hreg;
    
    ADD    #(.WIDTH(64)) add0(a, b, d);
    ADD    #(.WIDTH(64)) add1(a, c, e);
    SUB    #(.WIDTH(64)) sub0(a, b, f);
    COMP   #(.WIDTH(64)) comp0(d, e, u0a, u0b, dEQe);
    COMP   #(.WIDTH(64)) comp1(d, e, u1a, dLTe, u1b);
    MUX2x1 #(.WIDTH(64)) mux0(d, e, dLTe, g);
    MUX2x1 #(.WIDTH(64)) mux1(g, f, dEQe, h);
    REG    #(.WIDTH(64)) reg0(g, greg, clk, rst);
    REG    #(.WIDTH(64)) reg1(h, hreg, clk, rst);
    SHL    #(.WIDTH(64)) shl0(greg, dLTe, xrin);
    SHR    #(.WIDTH(64)) shr0(hreg, dEQe, zrin);
    REG    #(.WIDTH(64)) reg2(xrin, x, clk, rst);
    REG    #(.WIDTH(64)) reg3(zrin, z, clk, rst);
    
endmodule
