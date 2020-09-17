`timescale 1ns / 1ps

module SUB #(parameter WIDTH = 2) (a, b, diff);
    input [WIDTH-1:0] a, b;
    output reg [WIDTH-1:0] diff;

    always @(a, b) begin
		diff <= a - b;
	end
endmodule