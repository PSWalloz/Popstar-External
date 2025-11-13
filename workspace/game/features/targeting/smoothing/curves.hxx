#pragma once

namespace curves {
	inline std::vector<ImVec2> generate_smooth_curve( bool positive_curve, const ImVec2& start_point, const ImVec2& end_point, float distance, float smoothing ) {
		std::vector<ImVec2> control_points;
		control_points.push_back( start_point );

		ImVec2 direction = { end_point.x - start_point.x, end_point.y - start_point.y };

		ImVec2 perpendicular = { -direction.y, direction.x };
		float perp_length = sqrt( perpendicular.x * perpendicular.x + perpendicular.y * perpendicular.y );
		if ( perp_length > 1e-6f ) {
			perpendicular.x /= perp_length;
			perpendicular.y /= perp_length;
		}

		// More control points for longer distances to create deeper curves
		int num_control_points = std::clamp( static_cast< int >( distance / 50.0f ), 3, 6 );

		// Much more aggressive amplitude for deeper curves
		float base_amplitude = min( distance * 0.6f, 200.0f );
		float smoothing_multiplier = max( 0.3f, smoothing / 50.0f ); // More responsive to smoothing setting
		float max_amplitude = base_amplitude * smoothing_multiplier;
		float curve_direction = positive_curve ? 1.0f : -1.0f;

		for ( int i = 1; i <= num_control_points; i++ ) {
			float t = static_cast< float >( i ) / ( num_control_points + 1 );

			// Use single sine wave for more pronounced curves (not squared)
			float curve_factor = sin( t * 3.14159f );
			
			// Add extra curve intensity for middle points
			if ( t > 0.3f && t < 0.7f ) {
				curve_factor *= 1.5f; // Make middle section even more pronounced
			}
			
			float amplitude = max_amplitude * curve_factor * curve_direction;

			// More variation/noise for natural curves
			float noise = ( ( rand( ) % 100 - 50 ) / 100.0f ) * ( max_amplitude * 0.25f );
			amplitude += noise;

			ImVec2 control_point = {
				start_point.x + direction.x * t + perpendicular.x * amplitude,
				start_point.y + direction.y * t + perpendicular.y * amplitude
			};

			control_points.push_back( control_point );
		}

		control_points.push_back( end_point );
		return control_points;
	}

	inline ImVec2 calculate_bezier_point( const std::vector<ImVec2>& points, float t, ImVec2 default_pos ) {
		const size_t n = points.size( );

		if ( n == 0 ) {
			return default_pos;
		}

		if ( n == 1 ) {
			return points[ 0 ];
		}

		if ( n == 2 ) {
			const float t_inv = 1.0f - t;
			return ImVec2(
				t_inv * points[ 0 ].x + t * points[ 1 ].x,
				t_inv * points[ 0 ].y + t * points[ 1 ].y
			);
		}

		if ( n == 3 ) {
			const float t_inv = 1.0f - t;
			const float t_inv_sq = t_inv * t_inv;
			const float t_sq = t * t;
			const float two_t_inv_t = 2.0f * t_inv * t;

			return ImVec2(
				t_inv_sq * points[ 0 ].x + two_t_inv_t * points[ 1 ].x + t_sq * points[ 2 ].x,
				t_inv_sq * points[ 0 ].y + two_t_inv_t * points[ 1 ].y + t_sq * points[ 2 ].y
			);
		}

		if ( n == 4 ) {
			const float t_inv = 1.0f - t;
			const float t_inv_cu = t_inv * t_inv * t_inv;
			const float t_cu = t * t * t;
			const float three_t_inv_sq_t = 3.0f * t_inv * t_inv * t;
			const float three_t_inv_t_sq = 3.0f * t_inv * t * t;

			return ImVec2(
				t_inv_cu * points[ 0 ].x + three_t_inv_sq_t * points[ 1 ].x + three_t_inv_t_sq * points[ 2 ].x + t_cu * points[ 3 ].x,
				t_inv_cu * points[ 0 ].y + three_t_inv_sq_t * points[ 1 ].y + three_t_inv_t_sq * points[ 2 ].y + t_cu * points[ 3 ].y
			);
		}

		thread_local std::vector<ImVec2> temp_points;
		temp_points.resize( n );

		std::memcpy( temp_points.data( ), points.data( ), n * sizeof( ImVec2 ) );

		const float t_inv = 1.0f - t;

		for ( size_t level = 1; level < n; ++level ) {
			for ( size_t i = 0; i < n - level; ++i ) {
				temp_points[ i ].x = t_inv * temp_points[ i ].x + t * temp_points[ i + 1 ].x;
				temp_points[ i ].y = t_inv * temp_points[ i ].y + t * temp_points[ i + 1 ].y;
			}
		}

		return temp_points[ 0 ];
	}
}