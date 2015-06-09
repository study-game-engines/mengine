const char* accumulatorFS =
	"uniform sampler2D inSampler0;						"
	"varying mediump vec2 v_UV0;						"
	"varying lowp vec4 v_Col;							"
	"													"
	"void main(void)									"
	"{													"
	"	mediump vec4 c0 = texture2D(inSampler0, v_UV0);	"
	"	mediump vec4 c = c0;							"
	"	c.w *= v_Col.w;									"
	"	gl_FragColor = c;								"
	"}													";
