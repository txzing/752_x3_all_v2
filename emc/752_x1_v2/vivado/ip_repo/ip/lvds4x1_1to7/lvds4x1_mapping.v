`timescale 1ns / 1ns
`define UD #1
module lvds4x1_mapping
#(
parameter  DATA_FORMAT   = "PER_CLOCK"   // PER_CLOCK or PER_LINE data formatting  
)
( 
    input	[27:0]     din    ,
    output             VS0    ,
    output             HS0    ,
    output             DE0    ,
    output   [23:0]    dout0  
);								 
    //参数定义			 

    generate
        if (DATA_FORMAT == "PER_CLOCK") 
        begin    
            assign  VS0       = din[6 ];
            assign  HS0       = din[10];
            assign  DE0       = din[2 ];
            assign  dout0[0 ] = din[0 ];//G10
            assign  dout0[1 ] = din[25];//G11
            assign  dout0[2 ] = din[21];//G12
            assign  dout0[3 ] = din[17];//G13
            assign  dout0[4 ] = din[13];//G14
            assign  dout0[5 ] = din[9 ];//G15
            assign  dout0[6 ] = din[19];//G16
            assign  dout0[7 ] = din[15];//G17
            assign  dout0[8 ] = din[5 ];//B10
            assign  dout0[9 ] = din[1 ];//B11
            assign  dout0[10] = din[26];//B12
            assign  dout0[11] = din[22];//B13
            assign  dout0[12] = din[18];//B14
            assign  dout0[13] = din[14];//B15
            assign  dout0[14] = din[11];//B16
            assign  dout0[15] = din[7 ];//B17
            assign  dout0[16] = din[24];//R10
            assign  dout0[17] = din[20];//R11
            assign  dout0[18] = din[16];//R12
            assign  dout0[19] = din[12];//R13
            assign  dout0[20] = din[8 ];//R14
            assign  dout0[21] = din[4 ];//R15
            assign  dout0[22] = din[27];//R16
            assign  dout0[23] = din[23];//R17
        end
        else
        begin
            assign  VS0       = din[15];
            assign  HS0       = din[16];
            assign  DE0       = din[14];
            assign  dout0[0 ] = din[0 ];//G10
            assign  dout0[1 ] = din[13];//G11
            assign  dout0[2 ] = din[12];//G12
            assign  dout0[3 ] = din[11];//G13
            assign  dout0[4 ] = din[10];//G14
            assign  dout0[5 ] = din[9 ];//G15
            assign  dout0[6 ] = din[25];//G16
            assign  dout0[7 ] = din[24];//G17
            assign  dout0[8 ] = din[8 ];//B10
            assign  dout0[9 ] = din[7 ];//B11
            assign  dout0[10] = din[20];//B12
            assign  dout0[11] = din[19];//B13
            assign  dout0[12] = din[18];//B14
            assign  dout0[13] = din[17];//B15
            assign  dout0[14] = din[23];//B16
            assign  dout0[15] = din[22];//B17
            assign  dout0[16] = din[6 ];//R10
            assign  dout0[17] = din[5 ];//R11
            assign  dout0[18] = din[4 ];//R12
            assign  dout0[19] = din[3 ];//R13
            assign  dout0[20] = din[2 ];//R14
            assign  dout0[21] = din[1 ];//R15
            assign  dout0[22] = din[27];//R16
            assign  dout0[23] = din[26];//R17
        end 
    endgenerate

endmodule
