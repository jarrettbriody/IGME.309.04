#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);

out vec4 Fragment;

void main()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		Color = glm::vec3(1.0f - Color.r, 1.0f - Color.g, 1.0f - Color.b);
	Fragment = vec4(Color,1);
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
		Fragment = vec4(SolidColor, 1);

	return;
}