`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/30/2020 03:04:50 PM
// Design Name: 
// Module Name: circuit_3
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


module circuit_3(a, b, c, d, e, f, g, h, sa, avg, clk, rst);
    input clk, rst;
    input [15:0] a, b, c, d, e, f, g, h;
    input [7:0] sa;
    output [15:0] avg;
    
    wire [31:0] l00, l01, l02, l03, l10, l11, l2, l2div2, l2div4, l2div8;
    
    ADD #(.WIDTH(32)) add0(a, b, l00);
    ADD #(.WIDTH(32)) add1(c, d, l01);
    ADD #(.WIDTH(32)) add2(e, f, l02);
    ADD #(.WIDTH(32)) add3(g, h, l03);
    ADD #(.WIDTH(32)) add4(l00, l01, l10);
    ADD #(.WIDTH(32)) add5(l02, l03, l11);
    ADD #(.WIDTH(32)) add6(l10, l11, l2);
    SHR #(.WIDTH(32)) shr0(l2, sa, l2div2);
    SHR #(.WIDTH(32)) shr1(l2div2, sa, l2div4);
    SHR #(.WIDTH(32)) shr2(l2div4, sa, l2div8);
    REG #(.WIDTH(32)) reg0(l2div8, avg, clk, rst);

endmodule
