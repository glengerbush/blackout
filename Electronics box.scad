include<All.scad>
$fn=30;


%mirror([1,0,0])translate([-10,0,-8])cube([41,44.605,20]);

rotate([0,0,180])translate([25,-5,4])rotate([0,0,180])driver();
translate([-5,30,-5])rotate([0,0,0])bluefruit();
translate([0,0,0])rotate([0,0,0])Nano();
translate([-14,0,0])motorpowerhole();
translate([-30,0,-6])motorwirehole();



module motorpowerhole(){
	translate([0,0,0])rotate([90,0,0])cylinder(r=5.5/2+0.5,h=100,center=true);
	
}

module motorwirehole(){
	translate([0,-50,0])cube([11.5,100,6.5]);
	
}
	
module box(){
	translate([-9,0,-10-3.8/2])cube([25+1.7+9,29.21/2+30,20]);
}

module driver(){
    DRVxx=15.5;
    DRVyy=20.5;
    DRVzz=3.3;
    //DRVzz includes the tallest SMD component
    //
    translate([0,0,0])cube([DRVxx,DRVyy,DRVzz]);
    //trimpot, does not actually go above DRVzz
    //the motor connections are on 
    //the opposite side of the trimpot
    translate([DRVxx-3.9,3.45,(DRVzz+1)/2])cube([3.3,3.3,DRVzz+1],center=true);
    //plus the screw terminals!!!
    /*hull(){
        translate([-1.7,4.94,-6.6])cube([6.7,11.2,6.6]);
        translate([0,4.94,-10])cube([3.4,11.2,10]);
    }*/
}




module Nano(){
    Nanxx=18;
    Nanyy=43.5;
    Nanzz=1;
    translate([-Nanxx/2,0,0])cube([Nanxx,Nanyy,Nanzz]);
    //space for SMD components on bottom
    translate([0,(Nanyy-5.5)/2,-1.9/2])cube([Nanxx-2.2*2,Nanyy-5.5,1.9],center=true);
    //mini USB
    translate([0,9.25/2-1.75,3.9/2+Nanzz])cube([8,9.25,3.9],center=true);
    //reset button
    translate([0,25.7,Nanzz]){
        translate([0,0,1.63/2])cube([6.3,2.7,1.63],center=true);
        translate([0,0,1.63+0.6/2])cube([2.6,1.3,0.6],center=true);
    }
}


module bluefruit(){
    difference(){
        roundedBox([27.94,29.21,3.8],2,true);
        mirrorx()translate([22.86/2,29.21/2-6.35,0])cylinder(r=3.302/2,h=100,center=true);
        translate([0,-29.21/2+2.54,0])roundedBox([27.94-2.54*2,2,3.8*3],1,true);
    }
}
