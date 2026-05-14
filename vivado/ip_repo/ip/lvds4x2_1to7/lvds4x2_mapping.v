`timescale 1ns / 1ns
`define UD #1
module lvds4x2_mapping
#(
parameter  DATA_FORMAT   = "PER_CLOCK"   // PER_CLOCK or PER_LINE data formatting  
)
( 
    input	[55:0]     din    ,
    (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output             VS0    ,
    output             VS1    ,
    (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output             HS0    ,
    output             HS1    ,
    (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output             DE0    ,
    output             DE1    ,
    (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output   [23:0]    dout0  ,
    output   [23:0]    dout1   
);								 
    //参数定义			 

    generate
        if (DATA_FORMAT == "PER_CLOCK") 
        begin    
            assign  VS0       = din[6 ];
            assign  VS1       = din[34];
            assign  HS0       = din[10];
            assign  HS1       = din[38];
            assign  DE0       = din[2 ];
            assign  DE1       = din[30];
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
            assign  dout1[0 ] = din[28];//G20
            assign  dout1[1 ] = din[53];//G21
            assign  dout1[2 ] = din[49];//G22
            assign  dout1[3 ] = din[45];//G23
            assign  dout1[4 ] = din[41];//G24
            assign  dout1[5 ] = din[37];//G25
            assign  dout1[6 ] = din[47];//G26
            assign  dout1[7 ] = din[43];//G27
            assign  dout1[8 ] = din[33];//B20
            assign  dout1[9 ] = din[29];//B21
            assign  dout1[10] = din[54];//B22
            assign  dout1[11] = din[50];//B23
            assign  dout1[12] = din[46];//B24
            assign  dout1[13] = din[42];//B25
            assign  dout1[14] = din[39];//B26
            assign  dout1[15] = din[35];//B27
            assign  dout1[16] = din[52];//R20
            assign  dout1[17] = din[48];//R21
            assign  dout1[18] = din[44];//R22
            assign  dout1[19] = din[40];//R23
            assign  dout1[20] = din[36];//R24
            assign  dout1[21] = din[32];//R25
            assign  dout1[22] = din[55];//R26
            assign  dout1[23] = din[51];//R27
        end
        else
        begin
            assign  VS0       = din[15];
            assign  VS1       = din[43];
            assign  HS0       = din[16];
            assign  HS1       = din[44];
            assign  DE0       = din[14];
            assign  DE1       = din[42];
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
            assign  dout1[0 ] = din[28];//G20
            assign  dout1[1 ] = din[41];//G21
            assign  dout1[2 ] = din[40];//G22
            assign  dout1[3 ] = din[39];//G23
            assign  dout1[4 ] = din[38];//G24
            assign  dout1[5 ] = din[37];//G25
            assign  dout1[6 ] = din[53];//G26
            assign  dout1[7 ] = din[52];//G27
            assign  dout1[8 ] = din[36];//B20
            assign  dout1[9 ] = din[35];//B21
            assign  dout1[10] = din[48];//B22
            assign  dout1[11] = din[47];//B23
            assign  dout1[12] = din[46];//B24
            assign  dout1[13] = din[45];//B25
            assign  dout1[14] = din[51];//B26
            assign  dout1[15] = din[50];//B27
            assign  dout1[16] = din[34];//R20
            assign  dout1[17] = din[33];//R21
            assign  dout1[18] = din[32];//R22
            assign  dout1[19] = din[31];//R23
            assign  dout1[20] = din[30];//R24
            assign  dout1[21] = din[29];//R25
            assign  dout1[22] = din[55];//R26
            assign  dout1[23] = din[54];//R27
        end 
    endgenerate

endmodule
