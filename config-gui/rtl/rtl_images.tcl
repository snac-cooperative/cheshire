# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1999-2000 Patzschke + Rasp Software AG
# 
# Description: Images used by the Runtime Library.
# -----------------------------------------------------------------------
package provide rtl_images 1.2;

if {![info exists ::fg]} {

    entry .e;

    set ::fg [.e cget -foreground];
    set ::bg [.e cget -background];

    destroy .e;
}

# Default cursor images used for the combobox widget.
# Note: these images do not work under MacOs.

# Create images for the widget
# Create Image from uuencoded GIF: 
# up.gif 
image create photo rtl_up -gamma 1.0 -data {
R0lGODlhCQAEAPAAAAAA/wD//yH5BAEAAAIALAAAAAAJAAQAxwAAAP////wD
+wAAAP///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgVAAUI
FABg4EAACBEaJKjwYMKEAgICADs=
====
};

# Create Image from uuencoded GIF: 
# down.gif 
image create photo rtl_down -gamma 1.0 -data {
R0lGODlhCQAEAPAAAAAA/wD//yH5BAEAAAIALAAAAAAJAAQAxwAAAP////wD
+wAAAP///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgVAAUA
GEgQgICDAgciPFjQ4EKBCwMCADs=
====
};


image create bitmap _menu\
	-background $bg -foreground $fg\
	-data {#define smenu_width 14
#define smenu_height 14
#define smenu_x_hot 0
#define smenu_y_hot 0
static unsigned char smenu_bits[] = {
   0x00, 0x00, 0xfe, 0x0f, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
} -maskdata {#define smenu_mask_width 14
#define smenu_mask_height 14
#define smenu_mask_x_hot 0
#define smenu_mask_y_hot 0
static unsigned char smenu_mask_bits[] = {
   0x00, 0x00, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f,
   0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
};

image create bitmap _mcheckbutton -background $bg -foreground $fg\
	-data {#define smcheck_width 14
#define smcheck_height 14
#define smcheck_x_hot 0
#define smcheck_y_hot 0
static unsigned char smcheck_bits[] = {
   0x21, 0x00, 0x92, 0x0f, 0x0c, 0x18, 0x0c, 0x18, 0x12, 0x18, 0xa1, 0x1f,
   0x00, 0x18, 0x00, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
}\
	-maskdata {#define smcheck_mask_width 14
#define smcheck_mask_height 14
#define smcheck_mask_x_hot 0
#define smcheck_mask_y_hot 0
static unsigned char smcheck_mask_bits[] = {
   0x21, 0x00, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xff, 0x1f,
   0x00, 0x18, 0x00, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
};

image create bitmap _mradiobutton -background $bg -foreground $fg\
	-data {#define smradio_width 14
#define smradio_height 14
#define smradio_x_hot 0
#define smradio_y_hot 0
static unsigned char smradio_bits[] = {
   0x80, 0x00, 0xc0, 0x0f, 0x63, 0x18, 0x36, 0x18, 0x1c, 0x18, 0xc8, 0x1f,
   0x00, 0x18, 0x00, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
}\
	-maskdata {#define smradio_mask_width 14
#define smradio_mask_height 14
#define smradio_mask_x_hot 0
#define smradio_mask_y_hot 0
static unsigned char smradio_mask_bits[] = {
   0x80, 0x00, 0xfe, 0x0f, 0xff, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f,
   0x00, 0x18, 0x00, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
};

image create bitmap _mcommand -background $bg -foreground $fg\
	-data {#define smcommand_width 14
#define smcommand_height 14
#define smcommand_x_hot 0
#define smcommand_y_hot 0
static unsigned char smcommand_bits[] = {
   0x00, 0x00, 0x0e, 0x00, 0x62, 0x1b, 0x62, 0x1b, 0x02, 0x00, 0xfe, 0x1f,
   0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
}\
	-maskdata {#define smcommand_mask_width 14
#define smcommand_mask_height 14
#define smcommand_mask_x_hot 0
#define smcommand_mask_y_hot 0
static unsigned char smcommand_mask_bits[] = {
   0x00, 0x00, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x0f, 0xfe, 0x1f,
   0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
};

image create bitmap _mseparator -background $bg -foreground $fg\
	-data {#define smsep_width 14
#define smsep_height 14
#define smsep_x_hot 0
#define smsep_y_hot 0
static unsigned char smsep_bits[] = {
   0x00, 0x00, 0xfe, 0x0f, 0x02, 0x18, 0x02, 0x18, 0x00, 0x00, 0xb6, 0x0d,
   0x00, 0x00, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
}\
	-maskdata {#define smsep_mask_width 14
#define smsep_mask_height 14
#define smsep_mask_x_hot 0
#define smsep_mask_y_hot 0
static unsigned char smsep_mask_bits[] = {
   0x00, 0x00, 0xfe, 0x0f, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x0f, 0xfe, 0x0f,
   0xfe, 0x0f, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0x02, 0x18, 0xfe, 0x1f,
   0xfc, 0x1f, 0x00, 0x00};
};

image create bitmap _mcascade -background $bg -foreground $fg\
	-data {#define scascade_width 14
#define scascade_height 14
#define scascade_x_hot 0
#define scascade_y_hot 0
static unsigned char scascade_bits[] = {
   0x00, 0x00, 0xfe, 0x03, 0x02, 0x02, 0xf2, 0x0a, 0x12, 0x1a, 0xd6, 0x1b,
   0x12, 0x18, 0xf2, 0x1f, 0x12, 0x18, 0x16, 0x18, 0x10, 0x18, 0xf0, 0x1f,
   0xe0, 0x1f, 0x00, 0x00};
}\
	-maskdata {#define scascade_mask_width 14
#define scascade_mask_height 14
#define scascade_mask_x_hot 0
#define scascade_mask_y_hot 0
static unsigned char scascade_mask_bits[] = {
   0x00, 0x00, 0xfe, 0x03, 0xfe, 0x0f, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x1f,
   0xf2, 0x1f, 0xf2, 0x1f, 0x12, 0x18, 0x16, 0x18, 0x10, 0x18, 0xf0, 0x1f,
   0xe0, 0x1f, 0x00, 0x00};
};
