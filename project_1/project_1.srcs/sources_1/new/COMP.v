`timescale 1ns / 1ps

module COMP(a, b, gt, lt, eq, DATAWIDTH);
    input DATAWIDTH;
    parameter DW = DATAWIDTH;
    input [DW-1:0] a;
    input signed [DW-1:0] b;
    output reg gt, lt, eq;
    
    always @(a, b) begin
        if($signed({1'b0,a}) < b) begin //convert unsigned to signed
            gt <= 0; lt <= 1; eq <= 0;
        end
        else if($signed({1'b0,a}) > b``) begin
            gt <= 1; lt <= 0; eq <= 0;
        end
        else begin
            gt <= 0; lt <= 0; eq <= 1;
        end
    end
endmodule
