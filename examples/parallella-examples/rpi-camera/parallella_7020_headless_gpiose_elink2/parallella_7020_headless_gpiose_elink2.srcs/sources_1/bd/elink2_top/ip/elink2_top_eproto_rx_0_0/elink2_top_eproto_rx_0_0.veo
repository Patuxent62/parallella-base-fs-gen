// (c) Copyright 1995-2015 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.

// IP VLNV: adapteva.com:Adapteva:eproto_rx:1.0
// IP Revision: 5

// The following must be inserted into your Verilog file for this
// core to be instantiated. Change the instance name and port connections
// (in parentheses) to your own signal names.

//----------- Begin Cut here for INSTANTIATION Template ---// INST_TAG
elink2_top_eproto_rx_0_0 your_instance_name (
  .rx_rd_wait(rx_rd_wait),        // output wire rx_rd_wait
  .rx_wr_wait(rx_wr_wait),        // output wire rx_wr_wait
  .emrx_access(emrx_access),      // output wire emrx_access
  .emrx_write(emrx_write),        // output wire emrx_write
  .emrx_datamode(emrx_datamode),  // output wire [1 : 0] emrx_datamode
  .emrx_ctrlmode(emrx_ctrlmode),  // output wire [3 : 0] emrx_ctrlmode
  .emrx_dstaddr(emrx_dstaddr),    // output wire [31 : 0] emrx_dstaddr
  .emrx_srcaddr(emrx_srcaddr),    // output wire [31 : 0] emrx_srcaddr
  .emrx_data(emrx_data),          // output wire [31 : 0] emrx_data
  .reset(reset),                  // input wire reset
  .rxlclk_p(rxlclk_p),            // input wire rxlclk_p
  .rxframe_p(rxframe_p),          // input wire [7 : 0] rxframe_p
  .rxdata_p(rxdata_p),            // input wire [63 : 0] rxdata_p
  .emrx_rd_wait(emrx_rd_wait),    // input wire emrx_rd_wait
  .emrx_wr_wait(emrx_wr_wait)    // input wire emrx_wr_wait
);
// INST_TAG_END ------ End INSTANTIATION Template ---------

