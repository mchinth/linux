/****
    Copyright (C) 2019 Intel Corporation.  All Rights Reserved.

    This file is part of SEP Development Kit.

    SEP Development Kit is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    SEP Development Kit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
****/









#ifndef _PMU_INFO_PCI_H_INC_
#define _PMU_INFO_PCI_H_INC_

static U16 common_reg_list[] = {
	0xa0, 0xa8, 0xb0, 0xb8, 0xd8, 0xdc, 0xe0, 0xe4, 0xf4, 0xf8, 0x0
};

static U16 knl_pmu1_reg_list[] = {
	0x400, 0x408, 0x410, 0x418, 0x420, 0x424, 0x428, 0x42c, 0x430, 0x434,
	0x44c, 0x454, 0x0
};

static U16 common_pmu2_reg_list[] = {
	0xa0, 0xa8, 0xb0, 0xb8, 0xd0, 0xd8, 0xdc, 0xe0, 0xe4, 0xf0, 0xf4, 0xf8,
	0x0
};

static U16 knl_pmu2_reg_list[] = {
	0xb00, 0xb08, 0xb10, 0xb18, 0xb20, 0xb24, 0xb28, 0xb2c, 0xb30, 0xb34,
	0xb3c, 0xb44, 0x0
};

static U16 pmu2_dimm_reg_list[] = { 0x80, 0x84, 0x88, 0x0 };

static U16 pmu2_dimm_reg_list1[] = { 0xD0, 0xD8, 0xDC, 0xE0, 0xE4, 0x0 };

static U16 pmu12_dimm_reg_list[] = { 0xD4, 0x0 };

static U16 pmu3_reg_list1[] = { 0x3f4, 0x0 };

static U16 pmu3_reg_list2[] = { 0x150, 0x154, 0x0 };

static U16 pmu3_reg_list3[] = { 0x190, 0x194, 0x198, 0x19C, 0x1A0, 0x0 };

static U16 knl_pmu4_reg_list[] = {
	0xa00, 0xa08, 0xa10, 0xa18, 0xa20, 0xa24, 0xa28, 0xa2c, 0xa30, 0xa34,
	0xa3c, 0xa44, 0x0
};

static U16 pmu5_rx_tx_reg_list[] = {
	0x200, 0x204, 0x210, 0x214, 0x228, 0x22c, 0x238, 0x23c, 0x0 };

static U16 pmu5_lp_reg_list[] = { 0x4c, 0x50, 0x54, 0x58, 0xC0, 0x0 };

static U16 skx_pmu7_reg_list[] = {
	0x318, 0x320, 0x328, 0x330, 0x350, 0x358, 0x360, 0x368, 0x37c, 0x378, 0x0
};

static U16 icx_r_pmu7_reg_list[] = {
	0x320, 0x328, 0x330, 0x338, 0x350, 0x358, 0x360, 0x368, 0x37c, 0x318, 0x0
};

static U16 plat4_pmu7_reg_list[] = {
	0x320, 0x328, 0x330, 0x338, 0x340, 0x350, 0x358, 0x360, 0x368, 0x370,
	0x378, 0x37c, 0x380, 0x384, 0x388, 0x38c, 0x318, 0x0
};

static U16 pmu7_rx_tx_reg_list[] = {
	0x200, 0x204, 0x210, 0x214, 0x23C, 0x240, 0x244, 0x248, 0x24c, 0x250,
	0x254, 0x258, 0x25c, 0x260, 0x264, 0x268, 0x26C, 0x270, 0x0
};

static U16 pmu7_lp_reg_list[] = { 0x94, 0x120, 0x0 };

static U16 knl_pmu9_reg_list[] = { 0xa0, 0xa8, 0xd8, 0xdc, 0xf0, 0xf4, 0x0 };

static U16 common_pmu6_reg_list[] = {
	0xa0, 0xa8, 0xb0, 0xd8, 0xdc, 0xe0, 0xf4, 0xf8, 0x0
};

static U16 icx_pmu8_reg_list[] = {
	0xa0, 0xa8, 0xb0, 0xb8, 0xc0, 0xd8, 0xdc, 0xe0, 0xe4, 0xf8, 0x0
};

static U16 plat4_pmu8_reg_list[] = {
	0xa0, 0xa8, 0xb0, 0xb8, 0xc0, 0xd8, 0xe0, 0xe8, 0xf0, 0xf8, 0x0
};

static U16 skx_pmu10_reg_list[] = {
	0x200, 0x208, 0x210, 0x218, 0x228, 0x230, 0x238, 0x240, 0x258, 0x0
};

static U16 icx_r_pmu10_reg_list[] = {
	0x438, 0x440, 0x448, 0x450, 0x458, 0x460, 0x468, 0x470, 0x478, 0x480,
	0x488, 0x0
};

static U16 pmu11_reg_list[] = { 0x2A0, 0x2A4, 0x2A8, 0x2AC, 0x0 };

static PMU_PCI_UNIT_INFO_NODE hsx_pci_list[] = {
	{ 18, 1, common_reg_list      }, { 18, 5, common_reg_list      },
	{ 20, 0, common_pmu2_reg_list }, { 20, 1, common_pmu2_reg_list },
	{ 21, 0, common_pmu2_reg_list }, { 21, 1, common_pmu2_reg_list },
	{ 23, 0, common_pmu2_reg_list }, { 23, 1, common_pmu2_reg_list },
	{ 24, 0, common_pmu2_reg_list }, { 24, 1, common_pmu2_reg_list },
	{ 19, 2, pmu2_dimm_reg_list   }, { 19, 3, pmu2_dimm_reg_list   },
	{ 19, 4, pmu2_dimm_reg_list   }, { 19, 5, pmu2_dimm_reg_list   },
	{ 22, 2, pmu2_dimm_reg_list   }, { 22, 3, pmu2_dimm_reg_list   },
	{ 22, 4, pmu2_dimm_reg_list   }, { 22, 5, pmu2_dimm_reg_list   },
	{ 8,  2, common_reg_list      }, { 9,  2, common_reg_list      },
	{ 10, 2, common_reg_list      }, { 8,  6, pmu5_rx_tx_reg_list  },
	{ 9,  6, pmu5_rx_tx_reg_list  }, { 10, 6, pmu5_rx_tx_reg_list  },
	{ 8,  0, pmu5_lp_reg_list     }, { 9,  0, pmu5_lp_reg_list     },
	{ 10, 0, pmu5_lp_reg_list     }, { 5,  6, common_reg_list      },
	{ 16, 1, common_reg_list      }, { 11, 1, common_pmu6_reg_list },
	{ 11, 2, common_pmu6_reg_list }, { 11, 5, common_pmu6_reg_list },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE bdw_de_pci_list[] = {
	{ 18, 1, common_reg_list      }, { 18, 0, common_reg_list      },
	{ 20, 0, common_pmu2_reg_list }, { 20, 1, common_pmu2_reg_list },
	{ 21, 0, common_pmu2_reg_list }, { 21, 1, common_pmu2_reg_list },
	{ 23, 0, common_pmu2_reg_list }, { 23, 1, common_pmu2_reg_list },
	{ 24, 0, common_pmu2_reg_list }, { 24, 1, common_pmu2_reg_list },
	{ 19, 2, pmu2_dimm_reg_list   }, { 19, 3, pmu2_dimm_reg_list   },
	{ 19, 4, pmu2_dimm_reg_list   }, { 19, 5, pmu2_dimm_reg_list   },
	{ 5,  6, common_reg_list      }, { 16, 0, common_reg_list      },
	{ 16, 1, common_reg_list      }, { 11, 1, common_reg_list      },
	{ 11, 2, common_reg_list      }, { 11, 5, common_reg_list      },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE bdx_pci_list[] = {
	{ 18, 1, common_reg_list      }, { 18, 5, common_reg_list      },
	{ 20, 0, common_pmu2_reg_list }, { 20, 1, common_pmu2_reg_list },
	{ 21, 0, common_pmu2_reg_list }, { 21, 1, common_pmu2_reg_list },
	{ 23, 0, common_pmu2_reg_list }, { 23, 1, common_pmu2_reg_list },
	{ 24, 0, common_pmu2_reg_list }, { 24, 1, common_pmu2_reg_list },
	{ 19, 2, pmu2_dimm_reg_list   }, { 19, 3, pmu2_dimm_reg_list   },
	{ 19, 4, pmu2_dimm_reg_list   }, { 19, 5, pmu2_dimm_reg_list   },
	{ 22, 2, pmu2_dimm_reg_list   }, { 22, 3, pmu2_dimm_reg_list   },
	{ 22, 4, pmu2_dimm_reg_list   }, { 22, 5, pmu2_dimm_reg_list   },
	{ 8,  2, common_reg_list      }, { 9,  2, common_reg_list      },
	{ 10, 2, common_reg_list      }, { 8,  6, pmu5_rx_tx_reg_list  },
	{ 9,  6, pmu5_rx_tx_reg_list  }, { 10, 6, pmu5_rx_tx_reg_list  },
	{ 8,  0, pmu5_lp_reg_list     }, { 9,  0, pmu5_lp_reg_list     },
	{ 10, 0, pmu5_lp_reg_list     }, { 5,  6, common_reg_list      },
	{ 16, 0, common_reg_list      }, { 16, 1, common_reg_list      },
	{ 11, 1, common_reg_list      }, { 11, 2, common_reg_list      },
	{ 11, 5, common_reg_list      },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE knl_pci_list[] = {
	{ 8,  2, knl_pmu2_reg_list }, { 8,  3, knl_pmu2_reg_list },
	{ 8,  4, knl_pmu2_reg_list }, { 9,  2, knl_pmu2_reg_list },
	{ 9,  3, knl_pmu2_reg_list }, { 9,  4, knl_pmu2_reg_list },
	{ 10, 0, knl_pmu1_reg_list }, { 11, 0, knl_pmu1_reg_list },
	{ 24, 2, knl_pmu4_reg_list }, { 25, 2, knl_pmu4_reg_list },
	{ 26, 2, knl_pmu4_reg_list }, { 27, 2, knl_pmu4_reg_list },
	{ 28, 2, knl_pmu4_reg_list }, { 29, 2, knl_pmu4_reg_list },
	{ 30, 2, knl_pmu4_reg_list }, { 31, 2, knl_pmu4_reg_list },
	{ 15, 0, knl_pmu1_reg_list }, { 16, 0, knl_pmu1_reg_list },
	{ 17, 0, knl_pmu1_reg_list }, { 18, 0, knl_pmu1_reg_list },
	{ 19, 0, knl_pmu1_reg_list }, { 20, 0, knl_pmu1_reg_list },
	{ 21, 0, knl_pmu1_reg_list }, { 22, 0, knl_pmu1_reg_list },
	{ 5,  6, knl_pmu9_reg_list }, { 12, 1, common_reg_list   },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE skx_pci_list[] = {
	{ 10, 2, common_pmu2_reg_list }, { 10, 6, common_pmu2_reg_list },
	{ 11, 2, common_pmu2_reg_list }, { 12, 2, common_pmu2_reg_list },
	{ 12, 6, common_pmu2_reg_list }, { 13, 2, common_pmu2_reg_list },
	{ 10, 0, pmu2_dimm_reg_list   }, { 10, 4, pmu2_dimm_reg_list   },
	{ 11, 0, pmu2_dimm_reg_list   }, { 12, 0, pmu2_dimm_reg_list   },
	{ 12, 4, pmu2_dimm_reg_list   }, { 13, 0, pmu2_dimm_reg_list   },
	{ 14, 0, skx_pmu7_reg_list    }, { 15, 0, skx_pmu7_reg_list    },
	{ 16, 0, skx_pmu7_reg_list    }, { 14, 0, pmu7_rx_tx_reg_list  },
	{ 15, 0, pmu7_rx_tx_reg_list  }, { 16, 0, pmu7_rx_tx_reg_list  },
	{ 14, 0, pmu7_lp_reg_list     }, { 15, 0, pmu7_lp_reg_list     },
	{ 16, 0, pmu7_lp_reg_list     }, { 21, 1, common_reg_list      },
	{ 22, 1, common_reg_list      }, { 23, 1, common_reg_list      },
	{ 22, 5, common_reg_list      }, { 18, 1, common_pmu6_reg_list },
	{ 18, 2, common_pmu6_reg_list }, { 18, 5, common_pmu6_reg_list },
	{ 8,  0, skx_pmu10_reg_list   }, { 9,  0, skx_pmu10_reg_list   },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE snr_pci_list[] = {
	{ 0,  1, pmu2_dimm_reg_list1  }, { 12, 0, icx_r_pmu10_reg_list },
	{ 13, 0, icx_r_pmu10_reg_list }, { 14, 0, icx_r_pmu10_reg_list },
	{ 15, 0, icx_r_pmu10_reg_list }, { 0,  0, pmu3_reg_list1       },
	{ 0,  1, pmu3_reg_list2       },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE icx_r_pci_list[] = {
	{ 0,  1, pmu2_dimm_reg_list1  }, { 2,  1, icx_r_pmu7_reg_list  },
	{ 3,  1, icx_r_pmu7_reg_list  }, { 4,  1, icx_r_pmu7_reg_list  },
	{ 2,  3, pmu7_rx_tx_reg_list  }, { 3,  7, pmu7_rx_tx_reg_list  },
	{ 4,  3, pmu7_rx_tx_reg_list  }, { 2,  1, pmu7_lp_reg_list     },
	{ 3,  1, pmu7_lp_reg_list     }, { 4,  1, pmu7_lp_reg_list     },
	{ 5,  1, icx_pmu8_reg_list    }, { 6,  1, icx_pmu8_reg_list    },
	{ 7,  1, icx_pmu8_reg_list    }, { 12, 0, icx_r_pmu10_reg_list },
	{ 13, 0, icx_r_pmu10_reg_list }, { 14, 0, icx_r_pmu10_reg_list },
	{ 15, 0, icx_r_pmu10_reg_list }, { 0,  0, pmu3_reg_list1       },
	{ 0,  1, pmu3_reg_list2       },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE cpx_pci_list[] = {
	{ 10, 2, common_pmu2_reg_list }, { 10, 6, common_pmu2_reg_list },
	{ 11, 2, common_pmu2_reg_list }, { 12, 2, common_pmu2_reg_list },
	{ 12, 6, common_pmu2_reg_list }, { 13, 2, common_pmu2_reg_list },
	{ 10, 0, pmu2_dimm_reg_list   }, { 10, 4, pmu2_dimm_reg_list   },
	{ 11, 0, pmu2_dimm_reg_list   }, { 12, 0, pmu2_dimm_reg_list   },
	{ 12, 4, pmu2_dimm_reg_list   }, { 13, 0, pmu2_dimm_reg_list   },
	{ 14, 0, skx_pmu7_reg_list    }, { 15, 0, skx_pmu7_reg_list    },
	{ 16, 0, skx_pmu7_reg_list    }, { 14, 4, skx_pmu7_reg_list    },
	{ 15, 4, skx_pmu7_reg_list    }, { 16, 4, skx_pmu7_reg_list    },
	{ 14, 0, pmu7_rx_tx_reg_list  }, { 14, 4, pmu7_rx_tx_reg_list  },
	{ 15, 0, pmu7_rx_tx_reg_list  }, { 15, 4, pmu7_rx_tx_reg_list  },
	{ 16, 0, pmu7_rx_tx_reg_list  }, { 16, 4, pmu7_rx_tx_reg_list  },
	{ 14, 0, pmu7_lp_reg_list     }, { 14, 4, pmu7_lp_reg_list     },
	{ 15, 0, pmu7_lp_reg_list     }, { 15, 4, pmu7_lp_reg_list     },
	{ 16, 0, pmu7_lp_reg_list     }, { 16, 4, pmu7_lp_reg_list     },
	{ 21, 1, common_reg_list      }, { 22, 1, common_reg_list      },
	{ 23, 1, common_reg_list      }, { 22, 5, common_reg_list      },
	{ 18, 1, common_pmu6_reg_list }, { 18, 2, common_pmu6_reg_list },
	{ 19, 1, common_pmu6_reg_list }, { 19, 2, common_pmu6_reg_list },
	{ 20, 1, common_pmu6_reg_list }, { 20, 2, common_pmu6_reg_list },
	{ 8,  0, skx_pmu10_reg_list   }, { 9,  0, skx_pmu10_reg_list   },
	// End of list
	{ 0 }
};

static PMU_PCI_UNIT_INFO_NODE plat4_pci_list[] = {
	{ 0,  1, pmu2_dimm_reg_list1  }, { 0,  1, pmu12_dimm_reg_list  },
	{ 1,  1, plat4_pmu7_reg_list  }, { 2,  1, plat4_pmu7_reg_list  },
	{ 3,  1, plat4_pmu7_reg_list  }, { 4,  1, plat4_pmu7_reg_list  },
	{ 1,  1, pmu7_rx_tx_reg_list  }, { 2,  1, pmu7_rx_tx_reg_list  },
	{ 3,  1, pmu7_rx_tx_reg_list  }, { 4,  1, pmu7_rx_tx_reg_list  },
	{ 1,  1, pmu7_lp_reg_list     }, { 2,  1, pmu7_lp_reg_list     },
	{ 3,  1, pmu7_lp_reg_list     }, { 4,  1, pmu7_lp_reg_list     },
	{ 5,  1, plat4_pmu8_reg_list  }, { 6,  1, plat4_pmu8_reg_list  },
	{ 7,  1, plat4_pmu8_reg_list  }, { 8,  1, plat4_pmu8_reg_list  },
	{ 12, 0, icx_r_pmu10_reg_list }, { 13, 0, icx_r_pmu10_reg_list },
	{ 14, 0, icx_r_pmu10_reg_list }, { 15, 0, icx_r_pmu10_reg_list },
	{ 12, 1, icx_r_pmu10_reg_list }, { 12, 2, icx_r_pmu10_reg_list },
	{ 12, 3, icx_r_pmu10_reg_list }, { 12, 4, icx_r_pmu10_reg_list },
	{ 13, 1, icx_r_pmu10_reg_list }, { 13, 2, icx_r_pmu10_reg_list },
	{ 13, 3, icx_r_pmu10_reg_list }, { 13, 4, icx_r_pmu10_reg_list },
	{ 14, 1, icx_r_pmu10_reg_list }, { 14, 2, icx_r_pmu10_reg_list },
	{ 14, 3, icx_r_pmu10_reg_list }, { 14, 4, icx_r_pmu10_reg_list },
	{ 15, 1, icx_r_pmu10_reg_list }, { 15, 2, icx_r_pmu10_reg_list },
	{ 15, 3, icx_r_pmu10_reg_list }, { 15, 4, icx_r_pmu10_reg_list },
	{ 29, 1, pmu11_reg_list       }, { 0,  0, pmu3_reg_list1       },
	{ 3,  0, pmu3_reg_list3       },
	// End of list
	{ 0 }
};

#endif

