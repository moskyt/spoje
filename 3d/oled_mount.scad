include <bosl2/std.scad>
$fn = 80;

eps = 0.01;

window_x = 34;
window_y = 28;
glass_x = 35;
glass_y = 25;
view_x = 35;
view_y = 20;
mount_x = 45;
mount_y = 45;
board_x = 37;
board_y = 35;

outer_z = 1.5;
inner_z = 2;

cover_z = 1;    
shift_window = 3;

back(100) 
union() {
difference() {
    cuboid([mount_x, mount_y, cover_z], anchor = BOTTOM);

    down(eps)
    back(shift_window)
    cuboid([view_x, view_y, cover_z+2*eps], anchor = BOTTOM);

    up(0.1)
    xcopies(n=2, l=31)
    ycopies(n=2, l=29)
    zcyl(d = 5, l = cover_z+2*eps, anchor=BOTTOM);

}
    back(shift_window) difference() {
        cuboid([window_x, window_y, 4.5], anchor = TOP, edges="Z", rounding=3.5);
        up(eps)
        cuboid([view_x, view_y, 4.5+2*eps], anchor = TOP);
    }

}

difference() {
    cuboid([mount_x, mount_y, outer_z+inner_z], anchor = BOTTOM);
    down(eps)
    cuboid([glass_x, glass_y, outer_z+inner_z+2*eps], anchor = BOTTOM);
    up(outer_z)
    cuboid([board_x, board_y, inner_z+2*eps], anchor = BOTTOM);

    down(eps)
    xcopies(n=2, l=31)
    ycopies(n=2, l=29)
    zcyl(d = 3, l = 8, anchor=BOTTOM);

    down(eps)
    xcopies(n=2, l=31)
    ycopies(n=2, l=29)
    zcyl(d = 5, l = outer_z-0.5, anchor=BOTTOM);
 
    down(eps)
    back(12)
    cuboid([15, 5, 100+2*eps], anchor = BOTTOM+FWD);
   
}

