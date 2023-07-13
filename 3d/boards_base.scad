include <bosl2/std.scad>
$fn = 80;

eps = 0.01;


hole_spacing = 77;
board_x = 53;
board_spacing = 10;
board_y = 83;
board_z = 2;

hole_d = 3.9;

base_x = 115;
base_y = 85;
base_z = board_z + 1;

difference() {
    cuboid([base_x, base_y, base_z], anchor = TOP);
    
    up(eps)
    xcopies(board_spacing/2 + board_x, 2) {
        cuboid([board_x, board_y, board_z], anchor = TOP);
        ycopies(hole_spacing, 2)
        zcyl(d = hole_d, l = base_z+2*eps, anchor = TOP);
    }
}

fwd(base_y/2)
left(base_x/2) {
    right(20) {
        difference() {
            cuboid([10, 10, base_z], anchor = TOP+BACK);
            fwd(5) up(eps)
            zcyl(d = 4, l = base_z+2*eps, anchor = TOP);
        }
    }
    right(75) {
        difference() {
            cuboid([10, 10, base_z], anchor = TOP+BACK);
            fwd(5) up(eps)
            zcyl(d = 4, l = base_z+2*eps, anchor = TOP);
        }
    }
}


    right(100) {
        xcopies(20, 2)
        difference() {
            union() {
                cuboid([10, 10, 5], anchor = TOP+BACK);
                up(1)
                cuboid([10, 1, 5], anchor = TOP+BACK);
                fwd(3.75)
                up(5)
                cuboid([10, 6.25, 5], anchor = TOP+BACK);
            }
            fwd(5) up(eps)
            zcyl(d = 3.9, l = 100);
        }
    }
