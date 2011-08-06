void onMessage(uint32 msg) {
	print("onMessage");
}
void onProximity() {
	print("onProximity");
}
void onTimer() {
	print("onTimer");
}
void onQuit() {
	print("onQuit");
}

void main() {
	int32 a = print("woo! script!");
	print("" + a);
	sleep(5000);
	print("after sleep");
}