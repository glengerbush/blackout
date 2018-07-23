include<All.scad>
thickness=5;

xx=50;
yy=60;
zz=30;
mountScrewR=2;
//space between gear and chainhold
sbgnch=1;
$fn=120;






frame();
chainhold();
fourcornermirror()translate([xx/2-2,yy/2-10,0])cubesupport(15,15,15,2);
%translate([0,0,-5])import("beaded_belt_gear_95_6mm.stl");



module frame(){
    difference(){
        roundedBox([xx,yy,zz],3,true);
        motorholes();
        translate([0,0,thickness/2+1])cube([xx-thickness*2,200,zz-thickness+2],center=true);
    }
}


module cubesupport(xxx,yyy,zzz,thick){
    translate([xxx/2,0,zzz/2])difference(){
        roundedBox([xxx,yyy,zzz],2,true);
        translate([thick,0,-thick])roundedBox([xxx,yyy-thick*2,zzz],2,false);
        translate([-xxx/2+thick*0.75,-yyy,-zzz/2])rotate([0,45,0])translate([0,0,-zzz])cube([xxx*2,yyy*3,zzz*3]);
        translate([0,0,0])cylinder(r=mountScrewR,h=200,center=true);
    }
}

module chainhold(){
    //18mm!? forgot what the 18mm was for
	//thickness for chain hold around gear
	wallthick=3;
	
    difference(){
		union(){
			mirrory()translate([0,yy/2-15/2,-4])roundedBox([18+thickness+2,15,zz-8],3,true);
			translate([0,0,-4])tube(36.1/2+sbgnch+wallthick,zz-8,wallthick,true);
		}
        translate([0,0,-2])rotate([90,0,0])roundedBox([18,zz-8-thickness*2,yy*3],3,true);
        translate([0,0,10+(zz-8-thickness*2)/2-2+0.3])cube([5,yy*3,20],center=true);
		cylinder(r=(36.1/2+sbgnch),h=zz*3, center=true);
		fourcornermirror()translate([31/2,31/2,-30])cylinder(r=3.2,h=26);
		
    }
}
