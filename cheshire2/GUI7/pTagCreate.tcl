# pTagCreate -- procedure to create tags for text widgets for display

proc pTagCreate {t_name} {

    # Set up display styles
    
    $t_name tag configure <normal> -font -*-Courier-Bold-R-Normal-*-140-*
    $t_name tag configure <italic> -font -*-Courier-Bold-O-Normal-*-140-*
    $t_name tag configure <big> -font  -*-Times-Medium-R-Normal-*-180-*
    $t_name tag configure <verybig> -font -*-Times-Medium-R-Normal-*-240-*
    $t_name tag configure <blue> -background white -foreground blue
    $t_name tag configure <raised> -background white -relief raised \
                -borderwidth 1
    $t_name tag configure <sunken> -background white -relief sunken \
                -borderwidth 1
    $t_name tag configure <bgstipple> -background black -borderwidth 0 \
        -bgstipple gray25
    $t_name tag configure <fgstipple> -fgstipple gray50
    $t_name tag configure <underline> -underline on

}




