// fragment
uniform sampler2D tex;
uniform sampler2D mask;

uniform vec2 contentScale;
uniform vec2 scrollOffset;

void main(void)
{
    vec2 c0 = gl_TexCoord[0].st;
    vec2 c1 = gl_TexCoord[0].st;

    vec4 rgbValue = texture2D( tex, c0  );
    vec4 alphaValue = texture2D( mask, c1 );
    gl_FragColor.rgb = rgbValue.rgb;
	if (alphaValue.b == 1)
		gl_FragColor.a = rgbValue.a;
	else
		gl_FragColor.a = 0;

}