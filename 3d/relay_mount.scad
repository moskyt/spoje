include <bosl2/std.scad>
$fn = 80;

eps = 0.01;

board_w = 52;
hole_spacing = 44;
base_hole_spacing = 30;

difference() {
    cuboid([ board_w, 10, 10], anchor = BOTTOM+BACK);
    up(5)
    back(eps)
    xcopies(hole_spacing, 2)
    ycyl(l = 10+2*eps, d = 2.5, anchor=BACK);
 
    fwd(4)
    down(eps)
    xcopies(base_hole_spacing, 2)
    zcyl(l = 10+2*eps, d = 4, anchor=BOTTOM);

    up(2)
    back(eps)
    cuboid([ board_w+2*eps, 8+eps, 8+eps], anchor = BOTTOM+BACK);
}

fwd(10)
difference() {
    cuboid([ board_w, 2, 10], anchor = BOTTOM+BACK);
    up(1)
    cuboid([ board_w-2, 2+eps, 10-1+eps], anchor = BOTTOM+BACK);
}