class Tree : Entity {
	Tree() {
		print("Tree constructor");
	}
	void create(uint32 x, uint32 y) {
		print("Creating a tree at (" + x + ", " + y + ")");
	}
}

void createTypes() {
	system.registerType("Tree");
}
