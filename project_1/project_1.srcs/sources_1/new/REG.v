`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/14/2020 07:04:59 PM
// Design Name: 
// Module Name: REG
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

module REG(d, q, clk, rst, DATAWIDTH);
    input DATAWIDTH;
    parameter DW = DATAWIDTH;
    input [DW-1:0] d;
    output reg [DW-1:0] q;
    input clk, rst;
    
    always @(posedge clk) begin
        if(rst == 1) //synchronous reset
            q <= (DW-1);
        else
            q <= d;
    end
endmodule