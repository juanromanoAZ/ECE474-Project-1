`timescale 1ns / 1ps

module MUX2x1 #(parameter WIDTH = 2) (a, b, sel, d);
    input [WIDTH-1:0] a, b, sel;
    output reg [WIDTH-1:0] d;

    always @(a,b,sel) begin
	   if (sel == 1) begin
		  d <= a;
	   end

	   else begin
		  d <= b;
	   end
    end
endmodule