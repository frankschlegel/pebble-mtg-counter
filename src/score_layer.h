struct ScoreLayer;
typedef struct ScoreLayer ScoreLayer;

typedef enum ScoreLayerOrientation {
	ScoreLayerOrientationNormal,
	ScoreLayerOrientationUpsideDown,
} ScoreLayerOrientation;


extern ScoreLayer* score_layer_life_opponent;
extern ScoreLayer* score_layer_life_player;


ScoreLayer* score_layer_create(GRect frame, uint8_t num_digits);
void score_layer_destroy(ScoreLayer* score_layer);

Layer* score_layer_get_layer(ScoreLayer* score_layer);

int score_layer_get_score(ScoreLayer* score_layer);
void score_layer_set_score(ScoreLayer* score_layer, int score);

ScoreLayerOrientation score_layer_get_orientation(ScoreLayer* score_layer);
void score_layer_set_orientation(ScoreLayer* score_layer, ScoreLayerOrientation orientation);
ScoreLayerOrientation score_layer_get_orientation_all();
void score_layer_set_orientation_all(ScoreLayerOrientation orientation);

void score_layer_set_corner_mask(ScoreLayer* score_layer, GCornerMask corner_mask);
