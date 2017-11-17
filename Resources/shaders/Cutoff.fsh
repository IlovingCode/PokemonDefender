#ifdef GL_ES
precision mediump float;
#endif

uniform vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform float fx_threshold;
uniform float fy_threshold;

void main()
{
	vec4 temp = vec4(1.0);
	
	if(v_texCoord.y < fy_threshold){
		temp.xyzw = v_fragmentColor;
	}
	
	gl_FragColor = temp * texture2D(CC_Texture0, v_texCoord);
	
	if(v_texCoord.x > fx_threshold){
		gl_FragColor = temp;
	}
}