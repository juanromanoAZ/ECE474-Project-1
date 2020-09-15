`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/14/2020 07:04:59 PM
// Design Name: 
// Module Name: SHR
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

module SHR(a, sh_amt, d, DATAWIDTH);
    input DATAWIDTH;
    parameter DW = DATAWIDTH;
    input [DW-1:0] a, sh_amt;
    output reg [DW-1:0] d;

    always @(a, sh_amt) begin
        d <= a >> sh_amt;
    end
endmodule